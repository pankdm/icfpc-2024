import { Problem, ProblemStats } from '../api/types'

export function getFullViewBox(problem: ProblemStats | Problem) {
  return [0, 0, problem.room_width, problem.room_height]
}
export function getFullScaleOffset() {
  return {
    scale: 1,
    offset: {
      x: 0,
      y: 0,
    },
  }
}

export function getZoomedViewBox(problem: ProblemStats) {
  const {
    stage_width,
    stage_height,
    stage_bottom_left: [stage_x, stage_y],
  } = problem
  const pad = 0.1
  return [
    stage_x - stage_width * pad,
    stage_y - stage_height * pad,
    stage_width * (1 + 2 * pad),
    stage_height * (1 + 2 * pad),
  ]
}

export function getZoomedRect(problem: ProblemStats) {
  const [x0, y0, w, h] = getZoomedViewBox(problem)
  return [x0, y0, x0 + w, y0 + h]
}

export function getZoomedScaleOffset(problem: ProblemStats) {
  const [zoom_x0, zoom_y0, zoom_x1, zoom_y1] = getZoomedRect(problem)
  const zoom_cx = (zoom_x1 + zoom_x0) / 2
  const zoom_cy = (zoom_y1 + zoom_y0) / 2
  const zoom_cdx = zoom_cx - problem.room_width / 2
  const zoom_cdy = zoom_cy - problem.room_height / 2
  const room_max = Math.max(problem.room_width, problem.room_height)
  const zoom_max = Math.max(zoom_x1 - zoom_x0, zoom_y1 - zoom_y0)
  const scale = room_max / zoom_max
  return {
    scale,
    offset: {
      x: zoom_cdx / room_max,
      y: zoom_cdy / room_max,
    },
  }
}
