import _ from 'lodash'
import { Helmet } from 'react-helmet'
import { useParams } from 'react-router-dom'
import { Center, Group, Title } from '@mantine/core'
import config from '../../config'

export default function ProblemInspector() {
  const { problemId: problemIdStr } = useParams()

  return (
    <Group h="100%" pos="relative">
      <Helmet>
        <title>
          Problem {problemIdStr} - {config.HTML_TITLE}
        </title>
      </Helmet>
      <Center>
        <Title order={1}>Under construction</Title>
      </Center>
    </Group>
  )
}
