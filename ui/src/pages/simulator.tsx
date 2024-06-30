import {
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
} from '@tabler/icons-react'

const $input = persistentAtom<string>('icfpc-2024:simulation-input', '')
const $varA = persistentAtom<string>('icfpc-2024:simulation-varA', '1')
const $varB = persistentAtom<string>('icfpc-2024:simulation-varB', '0')

const Cell = ({
  snapshot,
  coord: { x, y },
}: {
  snapshot: BoardStatus
  coord: Partial<Coord>
}) => {
  const val: string =
    snapshot.board.get({ x, y } as Coord)?.val?.toString() || '.'
  return (
    <Center sx={{ border: '1px solid #ddd' }}>
      {{
        '>': <IconArrowRight />,
        '<': <IconArrowLeft />,
        '^': <IconArrowUp />,
        v: <IconArrowDown />,
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
        '*': <IconX />,
        '/': <IconDivide />,
        '%': <IconPercentage />,
        '+': <IconPlus />,
        '-': <IconMinus />,
      }[val] || val}
    </Center>
  )
}

export default function Simulator() {
  const input = useStore($input)
  const varA = useStore($varA)
  const varB = useStore($varB)
  const [snapshots, setSnapshots] = useState<any[]>()
  const [step, setStep] = useState(0)
  // const maxStep = (snapshots && snapshots.length - 1) || 0
  // const decrStep = () => setStep(Math.max((step -= 1), 0))
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
  return (
    <Box w="100%" h="100%" sx={{ overflow: 'auto' }}>
      <Helmet>
        <title>Leaderboard - {config.HTML_TITLE}</title>
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

          {finalBoard && spaceUsed && (
            <Center>
              <SimpleGrid
                cols={spaceUsed.x}
                spacing={0}
                verticalSpacing={0}
                w={spaceUsed.x * 50}
                h={spaceUsed.y * 50}
              >
                {...range(spaceUsed.minY, spaceUsed.maxY + 1)
                  .map((y) =>
                    range(spaceUsed.minX, spaceUsed.maxX + 1).map((x) => [x, y])
                  )
                  .flat()
                  .map(([x, y]) => (
                    <Cell
                      key={`${x},${y}`}
                      snapshot={currentSnapshot}
                      coord={{ x, y }}
                    />
                  ))}
              </SimpleGrid>
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
          <Slider
            disabled={!snapshots?.length}
            min={1}
            max={snapshots?.length}
            value={step + 1}
            onChange={(v) => setStep(v - 1)}
          />
          {finalBoard && (
            <>
              <Title order={3}>
                Answer:{' '}
                {finalBoard?.history?.result
                  ? finalBoard.history.result.value
                  : 'N/A'}
              </Title>
              {spaceUsed && (
                <Group>
                  <Text>Ticks used: {spaceUsed?.ticks}</Text>
                  <Text>
                    Space used: {spaceUsed.x} x {spaceUsed.y}
                  </Text>
                </Group>
              )}
            </>
          )}
        </Stack>
      </Container>
    </Box>
  )
}
