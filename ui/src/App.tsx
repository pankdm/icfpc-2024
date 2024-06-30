import { Routes, Route } from 'react-router-dom'
import AppLayout from './components/AppLayout'
import Root from './pages/root'
import Interpreter from './pages/interpreter'
import Simulator from './pages/simulator'

function App() {
  return (
    <Routes>
      <Route path="/" element={<AppLayout />}>
        <Route index element={<Root />} />
        <Route path="/simulator" element={<Simulator />} />
        <Route path="/interpreter" element={<Interpreter />} />
        <Route path="*" element={<Root />} />
      </Route>
    </Routes>
  )
}

export default App
