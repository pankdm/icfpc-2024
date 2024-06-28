import { Routes, Route } from 'react-router-dom'
import AppLayout from './components/AppLayout'
import Problems from './pages/problems'
import ProblemInspector from './pages/problems/id'
import Scoreboard from './pages/scoreboard'
import Root from './pages/root'

function App() {
  return (
    <Routes>
      <Route path="/" element={<AppLayout />}>
        <Route index element={<Root />} />
        <Route path="/problems/:problemId" element={<ProblemInspector />} />
        <Route path="/problems" element={<Problems />} />
        <Route path="/scoreboard" element={<Scoreboard />} />
        <Route path="*" element={<Root />} />
      </Route>
    </Routes>
  )
}

export default App
