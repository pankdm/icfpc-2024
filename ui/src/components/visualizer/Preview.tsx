import { Box, MantineSize, Paper, PaperProps } from '@mantine/core'
import { Problem } from '../../api/types'
import { Rect } from './primitives'
import { Attendee, Pillar } from './elements'

export default function Preview({
  size,
  problem,
  ...props
}: { size: MantineSize; problem: Problem } & PaperProps) {
  const {
    room_width,
    room_height,
    stage_width,
    stage_height,
    stage_bottom_left: [stage_x, stage_y],
  } = problem
  const roomAspect = room_width / room_height
  let [width, height] =
    roomAspect >= 1
      ? [size, `calc(${size} / ${roomAspect})`]
      : [`calc(${size} * ${roomAspect})`, size]
  return (
    <Paper
      w={width}
      h={height}
      shadow="md"
      withBorder
      bg="#453f3f"
      pos="relative"
      radius={0}
      {...props}
    >
      <Box
        component="svg"
        w="100%"
        h="100%"
        viewBox={`0 0 ${room_width} ${room_height}`}
        transform="scale(1,-1)"
      >
        {/* stage */}
        <Rect
          x={stage_x}
          y={stage_y}
          width={stage_width}
          height={stage_height}
          color="blue.9"
        />

        {/* pillars */}
        {problem.pillars.map((p, idx) => (
          <Pillar key={idx} x={p.center[0]} y={p.center[1]} radius={p.radius} />
        ))}

        {/* attendees */}
        {problem.attendees.map((att, idx) => (
          <Attendee key={idx} x={att.x} y={att.y} />
        ))}
      </Box>
    </Paper>
  )
}
