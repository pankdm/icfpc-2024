import {
  Group,
  Center,
  Textarea,
  Loader,
  Text,
  Stack,
  Box,
  ThemeIcon,
  Button,
  Switch,
} from '@mantine/core'
import { useState } from 'react'
import API from '../api'
import TrafficLight from '../components/TrafficLight'
import {
  IconArrowDown,
  IconArrowLeft,
  IconArrowRight,
} from '@tabler/icons-react'
import { useStore } from '@nanostores/react'
import { $warpDecodedResponse, Toggle } from '../components/state/toggles'

export default function Interpreter() {
  const [englishInput, setEnglishInput] = useState('')
  const [loadingEnglish, setLoadingEnglish] = useState(false)
  const [alienInput, setAlienInput] = useState('')
  const [loadingAlien, setLoadingAlien] = useState(false)
  const [error, setError] = useState<any>(null)
  const handleInput = (lang: 'english' | 'alien') => async (ev: any) => {
    const input = ev.target.value
    const [setInput, setLoadingOther, setOtherInput, apiCall] =
      lang === 'english'
        ? [setEnglishInput, setLoadingAlien, setAlienInput, API.encode]
        : [setAlienInput, setLoadingEnglish, setEnglishInput, API.decode]
    setInput(input)
    setError(null)
    if (!input) {
      setOtherInput('')
      return
    }
    setLoadingOther(true)
    try {
      setOtherInput(await apiCall(input))
    } catch (error) {
      setError(error)
    }
    setLoadingOther(false)
  }
  const [alienResponse, setAlienResponse] = useState('')
  const [decodedResponse, setDecodedResponse] = useState('')
  const [loadingComms, setLoadingComms] = useState(false)
  const warpDecodedResponse = useStore($warpDecodedResponse)
  const sendComms = async (ev) => {
    ev.preventDefault()
    setLoadingComms(true)
    try {
      const response = await API.communicate(alienInput)
      setAlienResponse(response)
      const decodedResponse = await API.decode(response)
      setDecodedResponse(decodedResponse)
    } catch (err) {
      setDecodedResponse('')
      setAlienInput(`Error: ${err.message}`)
    }
    setLoadingComms(false)
  }
  return (
    <Center mih="100%" pos="relative" component="form" onSubmit={() => sendComms}>
      <TrafficLight
        pos="absolute"
        top={0}
        right={0}
        size="8rem"
        red={!!error}
        green={!error}
      />
      <Stack w="100%" maw={1600} align="center">
        <Group w="100%" position="center" grow>
          <Textarea
            label={
              <Group>
                <Text>Earth</Text>
                {loadingEnglish && <Loader size="xs" />}
              </Group>
            }
            styles={{
              input: {
                fontFamily: 'monospace',
              },
            }}
            placeholder="English translation"
            minRows={5}
            value={englishInput}
            onChange={handleInput('english')}
          />
          <ThemeIcon sx={{ flex: 0 }} variant="subtle">
            <IconArrowRight />
          </ThemeIcon>
          <Textarea
            label={
              <Group>
                <Text>Alien</Text>
                {loadingAlien && <Loader size="xs" />}
              </Group>
            }
            styles={{
              input: {
                fontFamily: 'monospace',
              },
            }}
            placeholder="Enter gibberish"
            minRows={5}
            value={alienInput}
            onChange={handleInput('alien')}
          />
        </Group>
        <IconArrowDown />
        <Button onClick={sendComms} type="submit" loading={loadingComms}>
          Communicate
        </Button>
        <Textarea
          w="100%"
          label={
            <Group>
              <Text>Alien response</Text>
              {loadingAlien && <Loader size="xs" />}
            </Group>
          }
          styles={{
            input: {
              fontFamily: 'monospace',
              overflow: 'auto',
              whiteSpace: 'nowrap',
            },
          }}
          placeholder="Gibberish"
          minRows={3}
          value={alienResponse}
        />
        <IconArrowDown />
        <Textarea
          w="100%"
          label={
            <Group w="100%" justify="apart">
              <Text>Decoded response</Text>
              <Toggle atom={$warpDecodedResponse} label="Warp text" />
            </Group>
          }
          styles={{
            input: {
              fontFamily: 'monospace',
              overflow: 'auto',
              whiteSpace: warpDecodedResponse === 'y' ? 'normal' : 'nowrap',
            },
          }}
          placeholder="English response"
          minRows={25}
          value={decodedResponse}
        />
      </Stack>
    </Center>
  )
}
