import { Helmet } from 'react-helmet'
import { Box, Container, Text } from '@mantine/core'
import { MantineReactTable as Table } from 'mantine-react-table'
import API, { useAPIData } from '../api'
import { Scoreboard } from '../api/types'
import { formatNumber, formatNumberExp } from '../utils/numbers'
import config from '../config'

export default function Leaderboard() {
  const { data, isLoading } = useAPIData<Scoreboard>({
    fetch: () => API.getScoreboard(),
  })
  return (
    <Box w="100%" h="100%" sx={{ overflow: 'auto' }}>
      <Helmet>
        <title>Leaderboard - {config.HTML_TITLE}</title>
      </Helmet>
      <Container maw={1200} p={0} h="100%">
        <Table
          enableRowNumbers
          mantinePaperProps={{
            radius: 0,
            mah: '100%',
          }}
          columns={[
            {
              accessorKey: 'username',
              header: 'Name',
              size: 120,
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
              accessorKey: 'score',
              header: 'Score',
              accessorFn: (row) => formatNumber(row.score),
            },
            {
              accessorFn: (row) => formatNumberExp(row.score),
              id: 'score-exp',
              header: 'Score Exp',
            },
          ]}
          enablePagination={false}
          enableHiding={false}
          enableFullScreenToggle={false}
          enableColumnActions={false}
          enableStickyHeader
          enableDensityToggle={false}
          state={{ isLoading, density: 'xs' }}
          data={data?.scoreboard || []}
        />
      </Container>
    </Box>
  )
}
