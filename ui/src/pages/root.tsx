import { Center, Stack, Text, Title } from '@mantine/core'
import { Link } from 'react-router-dom'
import TrafficLight from '../components/TrafficLight'
import { useState } from 'react'

export default function Root() {
  const [color, setColor] = useState('red')
  const toggleColor = () => {
    setColor({ red: 'yellow', yellow: 'green', green: 'red' }[color] as string)
  }
  return (
    <Center h="100%">
      <Stack align="center">
        <TrafficLight
          size="8rem"
          {...{ [color]: true }}
          onClick={toggleColor}
          sx={{ cursor: 'pointer' }}
        />
        <Title order={2}>Welcome to ICFPC-2024!</Title>
        <Text>Team: Snakes, Monkeys and Two Smoking Lambdas</Text>
        <Link to="/simulator">
          <Text>Go solve some problems</Text>
        </Link>
      </Stack>
    </Center>
  )
}
