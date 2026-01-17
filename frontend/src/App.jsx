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
  const [beds, setBeds] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetch("/beds.json")
      .then((res) => res.json())
      .then((data) => {
        setBeds(data.beds || []);
        setLoading(false);
      })
      .catch((err) => {
        console.error("Failed to load JSON:", err);
        setLoading(false);
      });
  }, []);

  const groupedBeds = beds.reduce((acc, bed) => {
    acc[bed.type] = acc[bed.type] || [];
    acc[bed.type].push(bed);
    return acc;
  }, {});

  const totalBeds = beds.length;
  const occupiedBeds = beds.filter((b) => b.occupied).length;
  const freeBeds = totalBeds - occupiedBeds;

  if (loading) {
    return <h3 style={{ textAlign: "center" }}>Loading hospital status…</h3>;
  }

  return (
    <div className="container">
      {/* Header */}
      <header className="header">
        <h1>Hospital Bed Allocation Dashboard</h1>
        <p>
          Real-time visualization of hospital bed availability based on
          severity-driven allocation and predictive discharge scheduling.
        </p>
      </header>

      {/* Summary Cards */}
      <div className="summary">
        <div className="card">
          <h3>Total Beds</h3>
          <p>{totalBeds}</p>
        </div>
        <div className="card free">
          <h3>Available</h3>
          <p>{freeBeds}</p>
        </div>
        <div className="card occupied">
          <h3>Occupied</h3>
          <p>{occupiedBeds}</p>
        </div>
      </div>

      {/* Legend */}
      <div className="legend">
        <span className="legend-item free">🟢 Available</span>
        <span className="legend-item occupied">🔴 Occupied</span>
      </div>

      {/* Bed Sections */}
      {Object.keys(BED_TYPES).map((type) => {
        const list = groupedBeds[type] || [];
        const occ = list.filter((b) => b.occupied).length;

        return (
          <section key={type} className="section">
            <h2>
              {BED_TYPES[type]} — {occ}/{list.length} occupied
            </h2>

            <div className="grid">
              {list.map((bed) => (
                <div
                  key={bed.id}
                  className={`bed ${bed.occupied ? "occupied" : "free"}`}
                  title={`Bed ${bed.id} | ${
                    bed.occupied ? "Occupied" : "Available"
                  }`}
                >
                  {bed.id}
                </div>
              ))}
            </div>
          </section>
        );
      })}

      {/* Footer */}
      <footer className="footer">
        <p>
          Backend: Pure C (DSA-based allocation & prediction) · Frontend: React
          visualization
        </p>
        <p>Data source: beds.json (file-based integration)</p>
      </footer>
    </div>
  );
}

export default App;
