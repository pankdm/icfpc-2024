import {
  ActionIcon,
  Box,
  Button,
  Center,
  Container,
  Group,
  SimpleGrid,
  Slider,
  Stack,
  Text,
  TextInput,
  Textarea,
  ThemeIcon,
  Title,
} from '@mantine/core'
import { Helmet } from 'react-helmet'
import { BoardStatus, Coord, simulate } from '../utils/simulate3d'
import config from '../config'
import { useState } from 'react'
import { persistentAtom } from '@nanostores/persistent'
import { useStore } from '@nanostores/react'
import { range } from 'lodash'
import {
  IconArrowDown,
  IconArrowLeft,
  IconArrowRight,
  IconArrowUp,
  IconClock,
  IconDivide,
  IconMinus,
  IconPercentage,
  IconPlus,
  IconX,
  IconZoomIn,
  IconZoomOut,
} from '@tabler/icons-react'
import { useHotkeys } from '@mantine/hooks'

const $input = persistentAtom<string>('icfpc-2024:simulation-input', '')
const $varA = persistentAtom<string>('icfpc-2024:simulation-varA', '1')
const $varB = persistentAtom<string>('icfpc-2024:simulation-varB', '0')

const COLORS = {
  MOVE: '#f20',
  BINARY: '#f20',
}

const Cell = ({
  snapshot,
  coord: { x, y },
}: {
  snapshot: BoardStatus
  coord: Partial<Coord>
}) => {
  const val: string =
    snapshot.board.get({ x, y } as Coord)?.val?.toString() || '.'
  const moveColor = (mx, my) =>
    snapshot.board.get({
      x: x - mx,
      y: y - my,
    } as Coord)?.val
      ? COLORS.MOVE
      : undefined
  const arithmeticsColor = () =>
    snapshot.board.get({
      x: x - 1,
      y,
    } as Coord)?.val &&
    snapshot.board.get({
      x: x,
      y: y - 1,
    } as Coord)?.val
      ? COLORS.BINARY
      : undefined
  return (
    <Center sx={{ border: '1px solid #ddd' }}>
      {{
        '>': <IconArrowRight color={moveColor(1, 0)} />,
        '<': <IconArrowLeft color={moveColor(-1, 0)} />,
        '^': <IconArrowUp color={moveColor(0, 1)} />,
        v: <IconArrowDown color={moveColor(0, -1)} />,
        '+': <IconPlus color={arithmeticsColor()} />,
        '-': <IconMinus color={arithmeticsColor()} />,
        '*': <IconX color={arithmeticsColor()} />,
        '/': <IconDivide color={arithmeticsColor()} />,
        '%': <IconPercentage color={arithmeticsColor()} />,
        '@': (
          <ThemeIcon
            variant={
              snapshot.warped &&
              snapshot.board.get({ x, y: y - 1 } as Coord)?.val
                ? 'gradient'
                : 'subtle'
            }
          >
            <IconClock />
          </ThemeIcon>
        ),
      }[val] || val}
    </Center>
  )
}

const SnapshotPreview = ({
  snapshot,
  spaceUsed,
  zoom,
}: {
  snapshot: BoardStatus
  spaceUsed: any
  zoom: number
}) => (
  <SimpleGrid
    cols={spaceUsed.x}
    spacing={0}
    verticalSpacing={0}
    w={spaceUsed.x * 50 * zoom}
    h={spaceUsed.y * 50 * zoom}
    sx={{ flexShrink: 0 }}
  >
    {...range(spaceUsed.minY, spaceUsed.maxY + 1)
      .map((y) => range(spaceUsed.minX, spaceUsed.maxX + 1).map((x) => [x, y]))
      .flat()
      .map(([x, y]) => (
        <Cell key={`${x},${y}`} snapshot={snapshot} coord={{ x, y }} />
      ))}
  </SimpleGrid>
)

export default function Simulator() {
  const input = useStore($input)
  const varA = useStore($varA)
  const varB = useStore($varB)
  const [snapshots, setSnapshots] = useState<any[]>()
  const [step, setStep] = useState(0)
  const currentSnapshot = snapshots?.[step]
  const [finalBoard, setFinalBoard] = useState('')
  const [spaceUsed, setSpaceUsed] = useState<any>()
  const handleClickSimulate = () => {
    const { finalBoard, historicalBoards, spaceUsed } = simulate(
      input,
      parseInt(varA),
      parseInt(varB)
    )
    setSpaceUsed(spaceUsed)
    setSnapshots(historicalBoards)
    setStep(historicalBoards.length - 1)
    setFinalBoard(finalBoard)
  }
  const [zoom, setZoom] = useState(1)
  const zoomIn = () => setZoom(zoom*1.2)
  const zoomOut = () => setZoom(zoom/1.2)
  useHotkeys([
    ['-', () => zoomOut()],
    ['=', () => zoomIn()],
    ['Backspace', () => setStep(0)],
    ['ArrowLeft', () => setStep(Math.max(step - 1, 0))],
    ['ArrowRight', () => setStep(Math.min(step + 1, snapshots?.length - 1))],
  ])
  return (
    <Box w="100%" h="100%" sx={{ overflow: 'auto' }}>
      <Helmet>
        <title>3D Simulator - {config.HTML_TITLE}</title>
      </Helmet>
      <Container>
        <Stack w="100%">
          <Textarea
            styles={{ input: { fontFamily: 'monospace' } }}
            label="Solution"
            placeholder="Input formatted setup"
            value={input}
            onChange={(ev) => $input.set(ev.target.value)}
            minRows={10}
          />
          <Group>
            <TextInput
              label="Variable A"
              value={varA}
              onChange={(ev) => $varA.set(ev.target.value)}
            />
            <TextInput
              label="Variable B"
              value={varB}
              onChange={(ev) => $varB.set(ev.target.value)}
            />
          </Group>
          <Button onClick={handleClickSimulate}>Simulate</Button>

          {finalBoard && (
            <>
              <Group position="apart">
                <Title order={3}>
                  Answer:{' '}
                  {finalBoard?.history?.result
                    ? finalBoard.history.result.value
                    : 'N/A'}
                </Title>
                <Group>
                  <Text>Zoom:</Text>
                  <ActionIcon onClick={() => setZoom(zoom / 1.2)}>
                    <IconZoomOut />
                  </ActionIcon>
                  <Text>{zoom.toFixed(2)}</Text>
                  <ActionIcon onClick={() => setZoom(zoom * 1.2)}>
                    <IconZoomIn />
                  </ActionIcon>
                </Group>
              </Group>
              {spaceUsed && (
                <Group position='apart'>
                <Group>
                  <Text>Time used: {spaceUsed?.ticks}</Text>
                  <Text>
                    Space used: {spaceUsed.x} x {spaceUsed.y}
                  </Text>
                  <Text>
                      Volume: {spaceUsed.x * spaceUsed.y * spaceUsed?.ticks}
                  </Text>
                  <Text>
                      Total ticks: {snapshots?.length}
                  </Text>
                  </Group>
                  <Group c='gray'>
                    <Text>Scrub with</Text>
                    <IconArrowLeft/>
                    <IconArrowRight/> and Backspace
                  </Group>
                </Group>
              )}
            </>
          )}
          <Slider
            disabled={!snapshots?.length}
            min={1}
            max={snapshots?.length}
            value={step + 1}
            onChange={(v) => setStep(v - 1)}
          />
          <Text align="center">Time {currentSnapshot?.t}</Text>
          <Text align="center">Tick {step}</Text>
          {finalBoard && spaceUsed && (
            <Center>
              <SnapshotPreview
                snapshot={currentSnapshot}
                spaceUsed={spaceUsed}
                zoom={zoom}
              />
            </Center>
          )}
          <Textarea
            styles={{ input: { fontFamily: 'monospace' } }}
            contentEditable={false}
            label={`Time ${currentSnapshot?.t}`}
            placeholder="Output will show here"
            value={currentSnapshot && currentSnapshot.board.toString()}
            minRows={15}
          />
        </Stack>
      </Container>
    </Box>
  )
}
