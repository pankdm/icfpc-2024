import { Helmet } from 'react-helmet'
import { Box, Container, Text } from '@mantine/core'
import { MantineReactTable as Table } from 'mantine-react-table'
import API, { useAPIData } from '../api'
import { Scoreboard as APIScoreboard } from '../api/types'
import { formatNumber, formatNumberExp } from '../utils/numbers'
import config from '../config'

export default function Scoreboard() {
  const { data, isLoading } = useAPIData<APIScoreboard>({
    fetch: () => API.getScoreboard(),
  })
  return (
    <Box w="100%" h="100%" sx={{ overflow: 'auto' }}>
      <Helmet>
        <title>Leaderboard - {config.HTML_TITLE}</title>
      </Helmet>
      <Container maw={1200} p={0} h="100%">
        <Table
          mantinePaperProps={{
            radius: 0,
            mah: '100%',
          }}
          columns={[
            {
              id: 'rank',
              header: 'Rank',
              accessorFn: (row) => row?.values?.[0],
              size: 10
            },
            {
              id: 'name',
              header: 'Name',
              size: 200,
              accessorFn: (row) => row?.values?.[1],
              Cell: ({ cell }) => (
                <Text
                  maw="40vw"
                  sx={{ overflow: 'hidden', textOverflow: 'ellipsis' }}
                >
                  {cell.getValue<string>()}
                </Text>
              ),
            },
            {
              id: 'score',
              header: 'Score',
              accessorFn: (row) => formatNumber(0),
            },
            {
              id: 'score-exp',
              header: 'Score Exp',
              accessorFn: (row) => formatNumberExp(0),
            },
          ]}
          enablePagination={false}
          enableHiding={false}
          enableFullScreenToggle={false}
          enableColumnActions={false}
          enableStickyHeader
          enableDensityToggle={false}
          state={{ isLoading, density: 'xs' }}
          data={data?.rows || []}
        />
      </Container>
    </Box>
  )
}
