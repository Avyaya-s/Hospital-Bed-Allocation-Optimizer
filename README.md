# Hospital Bed Allocation Optimizer

A severity-based hospital bed management system built as a DSA project. The backend is a C CLI application that uses core data structures — hash table, min-heap, queue, and enum state machine — to handle patient admission, discharge, and bed reallocation. The frontend is a React dashboard (BED-ORACLE v3.0) that visualizes live system state by polling JSON exports from the backend.

---

## Features

**Patient Admission**
- Accepts patient ID, severity (1–5), recovery score (1–5), and emergency flag
- Emergency patients are triaged via a decision engine before allocation
- Non-emergency patients who cannot be immediately allocated are added to a FIFO waiting queue

**Emergency Triage (Decision Engine)**
- `ADMIT` — a free bed of matching severity exists
- `RECOMMEND TRANSFER` — no free bed, but one is discharge-approved
- `REJECT` — ward is fully occupied with no pending discharges

**Bed State Machine**
- Three states: `FREE` → `OCCUPIED` → `DISCHARGE_APPROVED` → `FREE`
- Doctor approval is a two-step process: approve first, then finalize discharge
- On finalize, the system automatically scans the waiting queue and reallocates to the first matching severity patient

**Manual Override**
- Force-discharge any bed by ID, bypassing the approval workflow
- Triggers the same queue reallocation logic

**Discharge Prediction (Min-Heap)**
- Predicted stay = `60 × (6 - recovery_score) × (1 + 0.1 × (severity - 1))` seconds (simulation scale: 1 min ≈ 1 day)
- Min-heap root always holds the earliest predicted free bed

**Audit Logger**
- Circular event log (up to 100 entries) with timestamps
- Exports to `events.json` on every JSON sync

**JSON Export**
- `beds.json` — current state of all beds, the waiting queue array, and the heap array
- `events.json` — full audit trail of system events
- `sync_json.bat` copies both files to `frontend/public/` for the dashboard to consume

---

## Data Structures Used

| Structure | Usage |
|---|---|
| Hash Table | O(1) bed lookup by bed ID |
| Min-Heap | Discharge prediction — earliest free bed at root |
| Circular Queue | FIFO waiting list for non-emergency patients |
| Enum State Machine | Tri-state bed lifecycle (FREE / OCCUPIED / DISCHARGE_APPROVED) |

---

## Bed Types

| Severity | Ward |
|---|---|
| 1 | General Ward |
| 2 | Semi-Private |
| 3 | Private |
| 4 | ICU |
| 5 | Ventilator |

Total beds: **42** (distributed across all 5 types)

---

## Project Structure

```
├── backend/
│   ├── main.c           # CLI entry point and menu
│   ├── bed.c/h          # Bed struct, BedState enum, init
│   ├── patient.h        # Patient struct (severity, recovery_score, emergency)
│   ├── hash.c/h         # Hash table for bed lookup
│   ├── heap.c/h         # Max-heap (emergency priority)
│   ├── predict.c/h      # Min-heap (discharge prediction)
│   ├── queue.c/h        # Circular FIFO queue (waiting patients)
│   ├── json.c/h         # JSON export (beds + queue + heap + logs)
│   ├── decision.c/h     # Emergency triage decision engine
│   ├── logger.c/h       # Circular event logger
│   └── data/
│       ├── beds.json    # Live bed/queue/heap state
│       └── events.json  # Audit log
├── frontend/
│   ├── src/
│   │   ├── App.jsx      # BED-ORACLE v3.0 — four-view dashboard
│   │   └── App.css      # Dark theme design system
│   └── public/
│       ├── beds.json    # Synced copy for Vite dev server
│       └── events.json  # Synced copy for Vite dev server
└── sync_json.bat        # Copies backend/data/*.json → frontend/public/
```

---

## Getting Started

### Backend (C)

Compile all source files:

```bash
gcc backend/*.c -o backend/hospital.exe
```

Run:

```bash
./backend/hospital.exe
```

Use **Option 3** from the menu to export state to JSON, then run `sync_json.bat` to push the files to the frontend.

### Frontend (React + Vite)

```bash
cd frontend
npm install
npm run dev
```

Open `http://localhost:5173` in your browser.

The dashboard auto-polls `beds.json` and `events.json` every 2 seconds. Run the backend, export JSON, and sync to see live updates.

---

## Dashboard Views

| View | Description |
|---|---|
| **Bed Grid** | Real-time bed matrix grouped by ward — color-coded FREE / OCCUPIED / DISCHARGE APPROVED |
| **Waiting Queue** | FIFO queue of patients awaiting a bed |
| **Prediction (Heap)** | Min-heap array showing each bed's predicted days until discharge |
| **Audit Logs** | Full event trail — admissions, discharges, rejections, transfers, reallocations |

---

## Backend Menu Reference

```
1. Admit new patient
2. Show next predicted free bed (INFO)
3. Export bed status to JSON
4. Manage Bed State (Discharge Support)
5. Show waiting queue
6. Unallocate / discharge bed by bed ID (Manual Override)
7. Exit
```
