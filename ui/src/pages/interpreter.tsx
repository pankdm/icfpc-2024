import { Group, Center, Textarea, ThemeIcon, Loader, Text } from '@mantine/core'
// import { useDebouncedState } from '@mantine/hooks'
import { IconArrowLeft, IconArrowRight } from '@tabler/icons-react'
import { useState } from 'react'
import API from '../api'

export default function Interpreter() {
  const [english, setEnglish] = useState('')
  const [loadingEnglish, setLoadingEnglish] = useState(false)
  const [alien, setAlien] = useState('')
  const [loadingAlien, setLoadingAlien] = useState(false)
  const handleEnglishInput = async (ev: any) => {
    const english = ev.target.value
    setEnglish(english)
    if (!english) {
      setAlien('')
      return
    }
    setLoadingAlien(true)
    try {
      const { text } = await API.translateToAlien(english)
      setAlien(text)
    } catch (error) {
      console.error(error)
    }
    setLoadingAlien(false)
  }
  const handleAlienInput = async (ev: any) => {
    const alien = ev.target.value
    setAlien(alien)
    if (!alien) {
      setEnglish('')
      return
    }
    setLoadingEnglish(true)
    try {
      const { text } = await API.translateToEnglish(alien)
      setEnglish(text)
    } catch (error) {
      console.error(error)
    }
    setLoadingEnglish(false)
  }
  return (
    <Center h="100%">
      <Group w="100%" maw={2000} position="center" grow>
        <Textarea
          label={
            <Group>
              <Text>Alien</Text>
              {loadingAlien && <Loader size="xs" />}
            </Group>
          }
          placeholder="Enter gibberish"
          minRows={10}
          value={alien}
          onChange={handleAlienInput}
        />
        <Group w={80} position="center" sx={{ flexGrow: 0 }}>
          <ThemeIcon variant="subtle">
            <IconArrowLeft />
          </ThemeIcon>
          <ThemeIcon variant="subtle">
            <IconArrowRight />
          </ThemeIcon>
        </Group>
        <Textarea
          label={
            <Group>
              <Text>English</Text>
              {loadingEnglish && <Loader size="xs" />}
            </Group>
          }
          placeholder="English translation"
          minRows={10}
          value={english}
          onChange={handleEnglishInput}
        />
      </Group>
    </Center>
  )
}
