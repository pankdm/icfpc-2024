import { MantineSize, Paper, PaperProps } from '@mantine/core'
import { Problem } from '../../api/types'
import { Canvas, useFrame, useThree } from '@react-three/fiber'
import { Rect } from './primitives.3d'
import { Attendee, Pillar } from './elements.3d'

const CameraPositioner = ({
  roomWidth,
  roomHeight,
}: {
  roomWidth: number
  roomHeight: number
}) => {
  const camera = useThree(({ camera }) => camera)
  const viewport = useThree(({ viewport }) => viewport)
  useFrame(() => {
    const canvas = document.querySelector('#visualizer')
    camera.position.x = roomWidth / 2
    camera.position.y = roomHeight / 2
    if (canvas) {
      const correctZoom =
        viewport.aspect >= 1
          ? canvas.clientWidth / roomWidth
          : canvas.clientHeight / roomHeight
      console.log(viewport.aspect, camera.zoom, correctZoom)
      if (correctZoom !== camera.zoom) {
        camera.zoom = correctZoom
        camera.updateMatrix()
      }
    }
  })
  return null
}

export default function Visualizer({
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
  const rmin = Math.min(room_width, room_height)
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
      pos="relative"
      radius={0}
      onResize={console.log}
      {...props}
    >
      <Canvas
        id="visualizer"
        onResize={(...args) => {
          console.log(...args)
        }}
        orthographic
        camera={{
          position: [0, 0, 100],
          left: -room_width / 2,
          right: room_width / 2,
          top: room_height / 2,
          bottom: -room_height / 2,
          near: 1,
          far: 100000,
        }}
      >
        <CameraPositioner roomWidth={room_width} roomHeight={room_height} />
        <ambientLight intensity={1} color={'#fff'} />
        {/* room */}
        <Rect
          x={0}
          y={0}
          z={0}
          width={room_width}
          height={room_height}
          color="#444"
        />

        {/* stage */}
        <Rect
          x={stage_x}
          y={stage_y}
          z={1}
          width={stage_width}
          height={stage_height}
          color="rgb(49, 99, 170)"
        />

        {/* pillars */}
        {problem.pillars.map((p, idx) => (
          <Pillar key={idx} x={p.center[0]} y={p.center[1]} radius={p.radius} />
        ))}

        {/* attendees */}
        {problem.attendees.map((att, idx) => (
          <Attendee
            key={idx}
            x={att.x}
            y={att.y}
            radius={rmin * 0.0025}
            z={2}
          />
        ))}
      </Canvas>
    </Paper>
  )
}
