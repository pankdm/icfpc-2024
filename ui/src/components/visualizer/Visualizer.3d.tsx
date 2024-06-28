import { RefObject, useRef, useState } from 'react'
import { MantineSize, Paper, PaperProps } from '@mantine/core'
import { Problem, Solution } from '../../api/types'
import { Canvas, useFrame, useThree } from '@react-three/fiber'
import { Rect } from './primitives.3d'
import { Attendee, Musician, Pillar } from './elements.3d'
// import API from '../../api'

const CameraPositioner = ({
  roomWidth,
  roomHeight,
}: {
  roomWidth: number
  roomHeight: number
}) => {
  const { camera, viewport } = useThree()
  const rmax = Math.max(roomWidth, roomHeight)
  useFrame(() => {
    const canvas = document.querySelector('#visualizer')
    camera.position.x = roomWidth / 2
    camera.position.y = roomHeight / 2
    if (canvas) {
      const correctZoom =
        viewport.aspect >= 1
          ? canvas.clientWidth / rmax
          : canvas.clientHeight / rmax
      if (correctZoom !== camera.zoom) {
        camera.zoom = correctZoom
        camera.updateProjectionMatrix()
      }
    }
  })
  return null
}

const ScreenshotHandler = ({
  requestRef,
  onScreenshotCaptured,
}: {
  requestRef: RefObject<{ value: boolean }>
  onScreenshotCaptured?: (blob: Blob) => void
}) => {
  const three = useThree()
  useFrame(() => {
    if (requestRef.current?.value) {
      requestRef.current.value = false
      three.gl.render(three.scene, three.camera)
      ;(
        document.querySelector('#visualizer canvas') as HTMLCanvasElement
      ).toBlob((blob) => {
        if (blob) {
          onScreenshotCaptured?.(blob)
        }
      })
    }
  })
  return null
}

export default function Visualizer3D({
  size,
  problemId,
  problem,
  solution,
  ...props
}: {
  problemId: number
  size: MantineSize
  problem: Problem
  solution?: Solution | null
  zoomMode: String
} & PaperProps) {
  const {
    room_width,
    room_height,
    stage_width,
    stage_height,
    stage_bottom_left: [stage_x, stage_y],
  } = problem
  const rmin = Math.min(room_width, room_height)
  const [stageHovered, setStageHovered] = useState(false)
  const [stageHeld, setStageHeld] = useState(false)

  const screenshotRequestRef = useRef({ value: false })
  // const triggerScreenshot = () => (screenshotRequestRef.current.value = true)
  const handleScreenshot = (_blob: Blob) => {
    // API.uploadPreview(problemId, blob)
  }
  return (
    <Paper w={size} h={size} shadow="md" radius={0} {...props}>
      <Canvas
        id="visualizer"
        orthographic
        camera={{
          position: [0, 0, 100],
          left: -50,
          right: 50,
          top: 50,
          bottom: -50,
          near: 1,
          far: 100000,
        }}
        // onClick={triggerScreenshot}
      >
        <CameraPositioner roomWidth={room_width} roomHeight={room_height} />
        <ScreenshotHandler
          requestRef={screenshotRequestRef}
          onScreenshotCaptured={handleScreenshot}
        />
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
          onPointerEnter={() => setStageHovered(true)}
          onPointerLeave={() => setStageHovered(false)}
          onPointerDown={() => setStageHeld(true)}
          onPointerUp={() => setStageHeld(false)}
          x={stage_x}
          y={stage_y}
          z={1}
          width={stage_width}
          height={stage_height}
          color={
            (stageHeld && '#274672') || (stageHovered && '#284c7e') || '#3163aa'
          }
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

        {/* musicians */}
        {solution?.placements?.map(({ x, y }, idx) => (
          <Musician key={idx} x={x} y={y} radius={5} z={2} />
        ))}
      </Canvas>
    </Paper>
  )
}
