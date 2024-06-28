import _ from 'lodash'
import { Link } from 'react-router-dom'
import { Helmet } from 'react-helmet'
import { Container, Group, SegmentedControl, Space } from '@mantine/core'
import { MantineReactTable as Table, MRT_ColumnDef } from 'mantine-react-table'
import API, { useAPIData } from '../../api'
import { ProblemStats, ProblemsStats } from '../../api/types'
import { formatNumber } from '../../utils/numbers'
import ProblemPreview from '../../components/ProblemPreview'
import config from '../../config'

export default function Problems() {
  const { data, isLoading } = useAPIData<ProblemsStats>({
    fetch: () => API.getProblemsStats(),
  })
  // Table setup
  const columns: Record<
    string,
    MRT_ColumnDef<{ id: string } & ProblemStats>
  > = {
    id: {
      accessorKey: 'id',
      Cell: ({ cell }) => (
        <Link to={`/problems/${cell.getValue()}`}>
          <Space w="xs" /># {cell.getValue<string>()}
        </Link>
      ),
      header: 'id',
      size: 20,
    },
    preview: {
      id: 'preview',
      accessorFn: (row) =>
        row.id && (
          <Link to={`/problems/${row.id}`}>
            <Group p={0} spacing={0} position="center">
              <ProblemPreview
                problemId={row.id}
                problemStats={row as ProblemStats}
                size={'min(20vmin, 8rem)'}
              />
            </Group>
          </Link>
        ),
      header: 'Preview',
      size: 140,
    },
    score: {
      id: 'score',
      header: 'Score',
      size: 90,
      accessorKey: 'score',
      Cell: ({ cell }) => (
        <span> {formatNumber(cell.getValue<number>())} </span>
      ),
    },
  }
  return (
    <Container maw={2500} h="100%" p={0}>
      <Helmet>
        <title>Problems - {config.HTML_TITLE}</title>
      </Helmet>
      <Table
        renderTopToolbarCustomActions={() => (
          <Group p={0}>
            <SegmentedControl
              color="orange.5"
              size="sm"
              data={['Table', 'Previews']}
              value={'Table'}
              onChange={() => null}
            />
          </Group>
        )}
        mantinePaperProps={{
          radius: 0,
          mah: '100%',
        }}
        columns={[columns.id, columns.preview, columns.score]}
        enableStickyHeader
        enablePagination={false}
        enableDensityToggle={false}
        enableColumnActions={false}
        enableHiding={false}
        state={{ isLoading, density: 'xs' }}
        data={Object.values(data?.problems || {}) as ProblemStats[]}
      />
    </Container>
  )
}
