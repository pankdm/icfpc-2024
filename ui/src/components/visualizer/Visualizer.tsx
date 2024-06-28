import { memo } from 'react'
import { Box, MantineSize, Paper, PaperProps } from '@mantine/core'
import { Problem, ProblemStats, Solution } from '../../api/types'
import { Rect } from './primitives'
import { Attendee, Musician, Pillar } from './elements'
import { getFullViewBox, getZoomedViewBox } from '../../utils/camera'
import { API_URL } from '../../api'
import { $previewInstrumentsMode } from '../../state/renderMode'
import { useStore } from '@nanostores/react'

const getPreviewImageURL = (
  problemId: string | number,
  instrumentId: number,
  mode: 'linear' | 'log'
) => `${API_URL}/problems/${problemId}/instruments/${instrumentId}/lut/${mode}`

// using `memo` to optimize React re-rendering
const Attendees = memo(function Attendees({ problem }: { problem: Problem }) {
  const { room_width, room_height } = problem
  const rmin = Math.min(room_width, room_height)
  return (
    <>
      {problem.attendees.map((att, idx) => (
        <Attendee key={idx} x={att.x} y={att.y} radius={rmin * 0.0025} />
      ))}
    </>
  )
})

export default function Visualizer({
  size,
  problemId,
  hoveredInstrumentId = -1,
  previewInstrumentId = -1,
  problem,
  solution,
  problemStats,
  zoomMode,
  onPointerPositionChange,
  onHoverMusician,
  onBlurMusician,
  ...props
}: {
  size: MantineSize
  problemId: string | number
  problem: Problem
  hoveredInstrumentId?: number
  previewInstrumentId?: number
  problemStats?: ProblemStats | null
  solution?: Solution | null
  zoomMode: String
  onPointerPositionChange?: (position: [number, number]) => void
  onHoverMusician?: (instrumentId: number, musicianId: number) => void
  onBlurMusician?: (instrumentId: number, musicianId: number) => void
} & PaperProps) {
  const {
    room_width,
    room_height,
    stage_width,
    stage_height,
    stage_bottom_left: [stage_x, stage_y],
  } = problem
  const zoomViewBox =
    problemStats && zoomMode === 'Zoom'
      ? getZoomedViewBox(problemStats)
      : getFullViewBox(problem)
  const previewInstrumentsMode = useStore($previewInstrumentsMode)
  return (
    <Paper
      w={size}
      h={size}
      shadow="md"
      withBorder
      pos="relative"
      radius={0}
      {...props}
    >
      <Box
        component="svg"
        w="100%"
        h="100%"
        viewBox={zoomViewBox.join(' ')}
        transform="scale(1,-1)"
      >
        {/* stage */}
        <Rect
          x={0}
          y={0}
          width={room_width}
          height={room_height}
          color="#453f3f"
        />
        <Rect
          x={stage_x}
          y={stage_y}
          width={stage_width}
          height={stage_height}
          color="blue.9"
        />
        {previewInstrumentId >= 0 &&
          previewInstrumentsMode !== 'musicians_only' && (
            <g transform={`translate(${stage_x},${stage_y})`}>
              <image
                x={0}
                y={0}
                width={stage_width}
                height={stage_height}
                transform={`scale(1,-1) translate(0,-${stage_height})`}
                href={getPreviewImageURL(
                  problemId,
                  previewInstrumentId,
                  previewInstrumentsMode
                )}
              />
            </g>
          )}

        {/* pillars */}
        {problem.pillars.map((p, idx) => (
          <Pillar key={idx} x={p.center[0]} y={p.center[1]} radius={p.radius} />
        ))}

        {/* attendees */}
        <Attendees problem={problem} />

        {/* musicians */}
        {solution?.placements?.map((p, idx) => {
          const instrumentId = problem.musicians[idx]
          return (
            <Musician
              key={idx}
              x={p.x}
              y={p.y}
              onMouseEnter={() => onHoverMusician?.(instrumentId, idx)}
              onMouseLeave={() => onBlurMusician?.(instrumentId, idx)}
              color={
                hoveredInstrumentId >= 0 && instrumentId === hoveredInstrumentId
                  ? '#f00'
                  : undefined
              }
              dimmed={
                previewInstrumentId >= 0 && instrumentId !== previewInstrumentId
              }
            />
          )
        })}
      </Box>
    </Paper>
  )
}
