import { useEffect, useState } from "react";
import "./App.css";

const BED_TYPES = {
  1: "General Ward",
  2: "Semi-Private",
  3: "Private",
  4: "ICU",
  5: "Ventilator",
};

function App() {
  const [bedsData, setBedsData] = useState({ beds: [], queue: [], heap: [] });
  const [logsData, setLogsData] = useState({ logs: [] });
  const [view, setView] = useState("grid"); // grid, queue, heap, logs
  const [loading, setLoading] = useState(true);

  const fetchData = () => {
    // Parallel fetch
    Promise.all([
      fetch(`/beds.json?t=${Date.now()}`).then(res => res.json()).catch(() => ({ beds: [], queue: [], heap: [] })),
      fetch(`/events.json?t=${Date.now()}`).then(res => res.json()).catch(() => ({ logs: [] }))
    ]).then(([bData, lData]) => {
      setBedsData(bData);
      setLogsData(lData);
      setLoading(false);
    });
  };

  useEffect(() => {
    fetchData();
    const interval = setInterval(fetchData, 2000); // Poll every 2s
    return () => clearInterval(interval);
  }, []);

  // --- VIEW COMPONENTS ---

  const BedGridView = () => {
    const groupedBeds = (bedsData.beds || []).reduce((acc, bed) => {
      acc[bed.type] = acc[bed.type] || [];
      acc[bed.type].push(bed);
      return acc;
    }, {});

    return (
      <div>
        <div className="view-header">
          <h2>Bed Grid (Operational View)</h2>
          <p>Real-time matrix of bed availability organized by ward type.</p>
        </div>

        {Object.keys(BED_TYPES).map((type) => {
          const list = groupedBeds[type] || [];
          return (
            <div key={type}>
              <div className="section-title">{BED_TYPES[type]}</div>
              <div className="bed-grid">
                {list.map((bed) => (
                  <div
                    key={bed.id}
                    className={`bed-cell ${bed.state === 0 ? "free" : bed.state === 1 ? "occupied" : "discharge"}`}
                    title={`Patient: ${bed.patient_id || "None"}`}
                  >
                    {bed.id}
                  </div>
                ))}
              </div>
            </div>
          );
        })}
      </div>
    );
  };

  const WaitingQueueView = () => {
    const queue = bedsData.queue || [];
    return (
      <div>
        <div className="view-header">
          <h2>Waiting Queue (FIFO)</h2>
          <p>Linear structure: First-In-First-Out for non-emergency patients.</p>
        </div>

        {queue.length === 0 ? (
          <div style={{ color: 'var(--text-secondary)', padding: '20px' }}>Queue is empty.</div>
        ) : (
          <div className="queue-container">
            <div style={{ fontSize: '12px', color: 'var(--text-secondary)', marginRight: '10px' }}>FRONT</div>
            {queue.map((p, idx) => (
              <div key={idx} className="queue-node">
                <div className="q-label">PATIENT</div>
                <div className="q-val">{p.patient_id}</div>
                <div className="q-sev">Sev: {p.severity}</div>
              </div>
            ))}
            <div style={{ fontSize: '12px', color: 'var(--text-secondary)', marginLeft: '10px' }}>REAR</div>
          </div>
        )}
      </div>
    );
  };

  const MinHeapView = () => {
    const heap = bedsData.heap || [];
    return (
      <div>
        <div className="view-header">
          <h2>Min-Heap (Prediction Engine)</h2>
          <p>Root (Index 0) = Earliest Predicted Discharge. <strong>Simulation Scale: 1 min = 1 day.</strong></p>
        </div>

        <div className="heap-array">
          {heap.map((node, idx) => {
            const remainingMins = Math.max(0, (node.discharge_time - Date.now() / 1000) / 60);
            // 1 min = 1 day logic
            const days = remainingMins.toFixed(1);

            return (
              <div key={idx} className="heap-node">
                <div className="h-idx">{idx}</div>
                <div className="h-val">Bed {node.bed_id}</div>
                <div className="h-sub" style={{ color: '#3b82f6' }}>~{days} days</div>
              </div>
            );
          })}
        </div>
        {heap.length === 0 && <div style={{ padding: '20px', color: 'var(--text-secondary)' }}>Heap is empty.</div>}
      </div>
    );
  };

  const LogView = () => {
    const logs = logsData.logs || [];
    // Let's reverse for "Latest Activity" feel.
    const displayLogs = [...logs].reverse();

    return (
      <div>
        <div className="view-header">
          <h2>System Event Log (Audit Trail)</h2>
          <p>Full chronological record of system actions and decisions.</p>
        </div>
        <table className="log-table">
          <thead>
            <tr>
              <th>Time</th>
              <th>Action</th>
              <th>Details</th>
            </tr>
          </thead>
          <tbody>
            {displayLogs.length === 0 ? (
              <tr><td colSpan="3" style={{ textAlign: 'center', padding: '20px', color: 'var(--text-secondary)' }}>No events recorded yet.</td></tr>
            ) : (
              displayLogs.map((log, idx) => (
                <tr key={idx} className="log-row">
                  <td style={{ fontFamily: 'var(--font-mono)', whiteSpace: 'nowrap', color: 'var(--text-secondary)' }}>{log.timestamp}</td>
                  <td style={{ fontFamily: 'var(--font-mono)', fontWeight: 'bold', color: 'var(--accent)' }}>{log.action}</td>
                  <td>{log.message}</td>
                </tr>
              ))
            )}
          </tbody>
        </table>
      </div>
    );
  };

  if (loading) return <div style={{ padding: '40px', color: 'white' }}>Loading System...</div>;

  return (
    <div className="app-container">
      {/* Sidebar navigation */}
      <div className="sidebar">
        <h1>🏥 BED-ORACLE <span style={{ fontSize: '10px', color: 'var(--accent)' }}>v3.0</span></h1>

        <div className={`nav-item ${view === "grid" ? "active" : ""}`} onClick={() => setView("grid")}>
          Bed Grid
        </div>
        <div className={`nav-item ${view === "queue" ? "active" : ""}`} onClick={() => setView("queue")}>
          Waiting Queue
        </div>
        <div className={`nav-item ${view === "heap" ? "active" : ""}`} onClick={() => setView("heap")}>
          Prediction (Heap)
        </div>
        <div className={`nav-item ${view === "logs" ? "active" : ""}`} onClick={() => setView("logs")}>
          Audit Logs
        </div>

        <div style={{ marginTop: 'auto', fontSize: '11px', color: 'var(--text-secondary)' }}>
          <p>Optimized Hospital Bed Management System</p>
          <p>Scale: 1m = 1d</p>
          <div style={{ display: 'flex', alignItems: 'center', marginTop: '8px' }}>
            <span className={`status-dot ${bedsData.beds ? 'ok' : 'busy'}`}></span>
            <div style={{ fontStyle: 'italic', color: 'var(--text-secondary)' }}>
              Maintain Proper lifestyle and diet and stay away from hospitals
            </div>
          </div>
        </div>
      </div>

      {/* Main Content Pane */}
      <div className="main-content">
        {view === "grid" && <BedGridView />}
        {view === "queue" && <WaitingQueueView />}
        {view === "heap" && <MinHeapView />}
        {view === "logs" && <LogView />}
      </div>
    </div>
  );
}

export default App;
