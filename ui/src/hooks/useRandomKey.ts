import { useCallback, useState } from 'react'

export default function useRandomKey(): [number, () => void] {
  const [randomKey, setRandomKey] = useState(Math.random())
  const refreshKey = useCallback(() => {
    setRandomKey(Math.random())
  }, [])
  return [randomKey, refreshKey]
}
