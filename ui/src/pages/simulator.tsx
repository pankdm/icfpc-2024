import {
  Box,
  Button,
  Container,
  Group,
  Slider,
  Stack,
  TextInput,
  Textarea,
  Title,
} from '@mantine/core'
import { Helmet } from 'react-helmet'
import { simulate } from '../utils/simulate3d.js'
import config from '../config'
import { useState } from 'react'
import { persistentAtom } from '@nanostores/persistent'
import { useStore } from '@nanostores/react'

const $input = persistentAtom<string>('icfpc-2024:simulation-input', '')
const $varA = persistentAtom<string>('icfpc-2024:simulation-varA', '1')
const $varB = persistentAtom<string>('icfpc-2024:simulation-varB', '0')

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
  const handleClickSimulate = () => {
    const { finalBoard, historicalBoards } = simulate(
      input,
      parseInt(varA),
      parseInt(varB)
    )
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
          <Textarea
            styles={{ input: { fontFamily: 'monospace' } }}
            // disabled
            contentEditable={false}
            label={`Time ${currentSnapshot?.t}`}
            placeholder="Output will show here"
            value={currentSnapshot && currentSnapshot.boardToString()}
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
            <Title order={3}>
              Answer: {finalBoard?.result ? finalBoard?.result.value : 'N/A'}
            </Title>
          )}
        </Stack>
      </Container>
    </Box>
  )
}
