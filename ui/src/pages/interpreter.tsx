import {
  Group,
  Center,
  Textarea,
  Loader,
  Text,
  Stack,
  Box,
} from '@mantine/core'
import { useState } from 'react'
import API from '../api'
import TrafficLight from '../components/TrafficLight'

export default function Interpreter() {
  const [english, setEnglish] = useState('')
  const [loadingEnglish, setLoadingEnglish] = useState(false)
  const [error, setError] = useState<any>(null)
  const [alien, setAlien] = useState('')
  const [loadingAlien, setLoadingAlien] = useState(false)
  const handleEnglishInput = async (ev: any) => {
    const english = ev.target.value
    setEnglish(english)
    setError(null)
    if (!english) {
      setAlien('')
      return
    }
    setLoadingAlien(true)
    try {
      const { text } = await API.translateToAlien(english)
      setAlien(text)
    } catch (error) {
      setError(error)
    }
    setLoadingAlien(false)
  }
  const handleAlienInput = async (ev: any) => {
    const alien = ev.target.value
    setAlien(alien)
    setError(null)
    if (!alien) {
      setEnglish('')
      return
    }
    setLoadingEnglish(true)
    try {
      const { text } = await API.translateToEnglish(alien)
      setEnglish(text)
    } catch (error) {
      setError(error)
    }
    setLoadingEnglish(false)
  }
  return (
    <Center h="100%">
      <Stack w="100%" maw={2000} align='center'>
        <Group w='100%' position="center" grow>
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
            minRows={15}
            value={alien}
            onChange={handleAlienInput}
          />
          <Center sx={{ flexGrow: 0 }}>
            <TrafficLight size="8rem" red={!!error} green={!error} />
          </Center>
          <Textarea
            label={
              <Group>
                <Text>English</Text>
                {loadingEnglish && <Loader size="xs" />}
              </Group>
            }
            styles={{
              input: {
                fontFamily: 'monospace',
              },
            }}
            placeholder="English translation"
            minRows={15}
            value={english}
            onChange={handleEnglishInput}
          />
        </Group>
        <Text ff="monospace" color={error ? 'black' : 'gray.3'}>{error?.message || 'No errors' }</Text>
      </Stack>
    </Center>
  )
}
