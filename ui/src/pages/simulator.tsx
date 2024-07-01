import {
  ActionIcon,
  Alert,
  Box,
  BoxProps,
  Button,
  Center,
  Container,
  Group,
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
  IconMinus,
  IconPercentage,
  IconPlus,
  IconSlash,
  IconX,
  IconZoomIn,
  IconZoomOut,
} from '@tabler/icons-react'
import { useHotkeys } from '@mantine/hooks'
import { tsvToBoard } from '../utils/tsv_import'

const $tsvInput = persistentAtom<string>('icfpc-2024:simulation-tsvInput', '')
const $boardInput = persistentAtom<string>(
  'icfpc-2024:simulation-boardInput',
  ''
)
const $varA = persistentAtom<string>('icfpc-2024:simulation-varA', '1')
const $varB = persistentAtom<string>('icfpc-2024:simulation-varB', '0')

const COLORS = {
  MOVE: '#f20',
  BINARY: '#f20',
}

const GridPos: React.FC<
  {
    cellSize: number
    coord: { x: number; y: number }
  } & BoxProps
> = ({ cellSize, coord: { x, y }, children, ...props }) => (
  <Center
    pos="absolute"
    left={cellSize * x}
    top={cellSize * y}
    w={cellSize}
    h={cellSize}
    {...props}
  >
    {children}
  </Center>
)

const CellValue: React.FC<
  {
    snapshot: BoardStatus
    coord: Partial<Coord>
  } & BoxProps
> = ({ snapshot, coord: { x, y }, w, h, ...props }) => {
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
  const equalityColor = (equality: boolean) => {
    const A = snapshot.board.get({
      x: x - 1,
      y,
    } as Coord)?.val
    const B = snapshot.board.get({
      x: x,
      y: y - 1,
    } as Coord)?.val
    return A && B && (A.value === B.value) === equality
      ? COLORS.BINARY
      : undefined
  }
  return (
    <Center
      ff="monospace"
      fw="bold"
      sx={{
        backgroundColor: 'rgba(255,255,255,0.75)',
        boxShadow: '0px 0px 5px 5px rgba(255,255,255,0.75)',
        lineHeight: 1,
        borderRadius: '50%',
      }}
      {...props}
    >
      {{
        '>': <IconArrowRight color={moveColor(1, 0)} />,
        '<': <IconArrowLeft color={moveColor(-1, 0)} />,
        '^': <IconArrowUp color={moveColor(0, -1)} />,
        v: <IconArrowDown color={moveColor(0, 1)} />,
        '+': <IconPlus color={arithmeticsColor()} />,
        '-': <IconMinus color={arithmeticsColor()} />,
        '*': <IconX color={arithmeticsColor()} />,
        '/': <IconSlash color={arithmeticsColor()} />,
        '%': <IconPercentage color={arithmeticsColor()} />,
        '=': <Text color={equalityColor(true)}>=</Text>,
        '#': <Text color={equalityColor(false)}>#</Text>,
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
}) => {
  const meta = {
    warpDestinations: [...snapshot.board.values()]
      .filter((cell) => cell.val.opcode === '@')
      .map((cell) => ({
        from: cell.coord,
        to: {
          x:
            cell.coord.x -
            snapshot.board.get([cell.coord.x - 1, cell.coord.y]).val.value,
          y:
            cell.coord.y -
            snapshot.board.get([cell.coord.x + 1, cell.coord.y]).val.value,
        },
      })),
  }
  const cellSize = 50 * zoom
  return (
    <Box
      pos="relative"
      sx={{ border: '1px solid gray', flexShrink: 0 }}
      w={cellSize * spaceUsed.x}
      h={cellSize * spaceUsed.y}
    >
      {...range(spaceUsed.minY, spaceUsed.maxY + 1).map((y) => (
        <>
          {...range(spaceUsed.minX, spaceUsed.maxX + 1).map((x) => (
            <GridPos key={`${x},${y}`} cellSize={cellSize} coord={{ x, y }}>
              <Box
                sx={{ border: '1px solid gray' }}
                w={cellSize}
                h={cellSize}
              />
            </GridPos>
          ))}
        </>
      ))}
      {meta.warpDestinations.map(({ to }) => (
        <GridPos key={`${to.x},${to.y}`} cellSize={cellSize} coord={to}>
          <ThemeIcon variant="gradient" color="yellow" opacity={0.2} size="xl">
            <IconClock />
          </ThemeIcon>
        </GridPos>
      ))}
      {Array.from(snapshot.board.values()).map((cell) => (
        <GridPos
          key={`${cell.coord.x},${cell.coord.y}`}
          cellSize={cellSize}
          coord={cell.coord}
        >
          <CellValue snapshot={snapshot} coord={cell.coord} />
        </GridPos>
      ))}
    </Box>
  )
}

export default function Simulator() {
  const boardInput = useStore($boardInput)
  const tsvInput = useStore($tsvInput)
  const varA = useStore($varA)
  const varB = useStore($varB)
  const [error, setError] = useState(null);
  const [snapshots, setSnapshots] = useState<any[]>()
  const [step, setStep] = useState(0)
  const currentSnapshot = snapshots?.[step]
  const [finalBoard, setFinalBoard] = useState<any>()
  const [spaceUsed, setSpaceUsed] = useState<any>()
  const handleClickFormatTsvToInput = () => {
    setError(null)
    try {
      const input = tsvToBoard(tsvInput)
      $boardInput.set(input)
    } catch (err) {
      setError(err)
    }
  }
  const handleClickSimulate = () => {
    setError(null)
    try {
      const { finalBoard, historicalBoards, spaceUsed } = simulate(
        boardInput,
        parseInt(varA),
        parseInt(varB)
      )
      setSpaceUsed(spaceUsed)
      setSnapshots(historicalBoards)
      setStep(0)
      setFinalBoard(finalBoard)
    } catch (err) {
      setError(err)
    }
  }
  const [zoom, setZoom] = useState(1)
  const zoomIn = () => setZoom(zoom * 1.2)
  const zoomOut = () => setZoom(zoom / 1.2)
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
          <Group>
            <Textarea
              sx={{ flex: 1 }}
              styles={{ input: { fontFamily: 'monospace' } }}
              label="Spreadsheet input"
              placeholder="Paste from spreadsheet"
              value={tsvInput}
              onChange={(ev) => $tsvInput.set(ev.target.value)}
              minRows={10}
            />
            <ActionIcon sx={{ flexGrow: 0 }}>
              <IconArrowRight onClick={handleClickFormatTsvToInput} />
            </ActionIcon>
            <Textarea
              sx={{ flex: 1 }}
              styles={{ input: { fontFamily: 'monospace' } }}
              label="Board"
              placeholder="Input formatted board"
              value={boardInput}
              onChange={(ev) => $boardInput.set(ev.target.value)}
              minRows={10}
            />
          </Group>
          <Button onClick={handleClickSimulate}>Simulate</Button>
          {error && (
            <Alert color="red">
              <Text ff="monospace" sx={{whiteSpace: 'pre'}}>{error.stack}</Text>
            </Alert>
          )}
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
                <Group position="apart">
                  <Group>
                    <Text>Time used: {spaceUsed?.ticks}</Text>
                    <Text>
                      Space used: {spaceUsed.x} x {spaceUsed.y}
                    </Text>
                    <Text>
                      Volume: {spaceUsed.x * spaceUsed.y * spaceUsed?.ticks}
                    </Text>
                    <Text>Total ticks: {snapshots?.length - 1}</Text>
                  </Group>
                  <Group c="gray">
                    <Text>Scrub with</Text>
                    <IconArrowLeft />
                    <IconArrowRight /> and Backspace
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
