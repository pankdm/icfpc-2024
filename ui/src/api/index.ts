import { useCallback, useEffect, useState } from 'react'
import {
  Problem,
  ProblemStats,
  Problems,
  ProblemsStats,
  Scoreboard,
  Solution,
  Solutions,
  Userboard,
} from './types'
import useRandomKey from '../hooks/useRandomKey'

export const API_URL = import.meta.env.VITE_API_URL || 'http://localhost:8000'

const fetchAPI = async (
  path: string,
  requestOpts: Omit<RequestInfo, 'url'> = {}
) => {
  const { body, headers = {}, ...otherOpts } = requestOpts
  return fetch(`${API_URL}${path}`, {
    ...otherOpts,
    headers: {
      ...(body && { 'Content-Type': 'application/json' }),
      ...headers,
    },
    body: body && JSON.stringify(body),
  })
    .then(async (r) => {
      if (r.status >= 300) {
        throw new Error(await r.text())
      }
      if (r.headers.get('Content-Type') === 'application/json') {
        return await r.json()
      } else {
        return await r.text()
      }
    })
    .catch((err) => {
      console.error(err)
      throw err
    })
}

export const AssetURL = {
  hishogram: (problemId: string, type: string) =>
    `${API_URL}/problems/${problemId}/histograms/${type}`,
}

const API = {
  getScoreboard: async () => fetchAPI(`/scoreboard`) as Promise<Scoreboard>,
  getUserboard: async () => fetchAPI(`/userboard`) as Promise<Userboard>,
  getProblems: async () => fetchAPI(`/problems`) as Promise<Problems>,
  getProblemsStats: async () =>
    fetchAPI(`/problems/stats`) as Promise<ProblemsStats>,
  getProblemStats: async (problemId: string | number) =>
    fetchAPI(`/problems/${problemId}/stats`) as Promise<ProblemStats>,
  getProblem: async (problemId: number | string) =>
    fetchAPI(`/problems/${problemId}`) as Promise<Problem>,
  getProblemSolutions: async (problemId: number | string) =>
    fetchAPI(`/problems/${problemId}/solutions`) as Promise<Solutions>,
  getSolution: async (solutionId: string) =>
    fetchAPI(`/solutions/${solutionId}`) as Promise<Solution>,
  decode: async (text: string) =>
    fetchAPI(`/decode`, {
      method: 'POST',
      body: { text },
    }),
  encode: async (text: string) =>
    fetchAPI(`/encode`, {
      method: 'POST',
      body: { text },
    }),
  communicate: async (text: string) =>
    fetchAPI(`/communicate`, {
      method: 'POST',
      body: { text },
    }),
}

export function useAPIData<T extends Record<string, any>>({
  fetch,
  beforeFetch,
  onSuccess,
  skip = false,
  deps = [],
}: {
  fetch: () => Promise<T>
  beforeFetch?: () => any
  onSuccess?: (data: T) => any
  skip?: boolean | (() => boolean)
  deps?: any[]
}) {
  const [isLoading, setIsLoading] = useState(false)
  const [error, setError] = useState<Error | null>(null)
  const [data, setData] = useState<T | null>(null)
  const clearData = useCallback(() => {
    setData(null)
  }, [])
  const [fetchKey, refreshFetchKey] = useRandomKey()
  useEffect(() => {
    if ((skip as any)?.call ? (skip as () => boolean)() : skip) {
      data && setData(null)
      return
    }
    setIsLoading(true)
    setError(null)
    beforeFetch?.()
    fetch()
      .then((result) => {
        setData(result)
        onSuccess?.(result)
      })
      .catch((err) => {
        setError(err)
      })
      .finally(() => {
        setIsLoading(false)
      })
  }, [fetchKey, ...deps])
  return { isLoading, error, data, clearData, refetch: refreshFetchKey }
}

export default API
