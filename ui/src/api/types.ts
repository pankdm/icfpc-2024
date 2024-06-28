export type Scoreboard = {
  columns: string[]
  rows: {
    isYou: boolean
    values: any[]
  }
}

export type SubprocessResult = {
  status_code: number
  stdout?: string
  stderr?: string
}

export type Userboard = {
  problems: {
    id: number
    score: number | null
  }
}

export type Problems = {
  problems: number[]
}

export type ProblemStats = {
  score: number | null
  estimated_max: number | null
  our_best: any
}
export type ProblemsStats = {
  problems: {
    [id: string]: ProblemStats
  }
}

export type Problem = {
  // TBD
}

export type Solutions = {
  solutions: string[]
}

export type Solution = {
  // TBD
}
