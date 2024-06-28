import _ from 'lodash'
import { Link } from 'react-router-dom'
import { Helmet } from 'react-helmet'
import { useStore } from '@nanostores/react'
import {
  Box,
  Container,
  Group,
  Image,
  SegmentedControl,
  Space,
  Stack,
} from '@mantine/core'
import { MantineReactTable as Table, MRT_ColumnDef } from 'mantine-react-table'
import API, { AssetURL, useAPIData } from '../../api'
import { ProblemStats } from '../../api/types'
import { formatNumber, formatNumberExp } from '../../utils/numbers'
import ProblemPreview from '../../components/ProblemPreview'
import config from '../../config'
import {
  $userboardDisplayMode,
  $userboardZoomMode,
} from '../../state/userboardDisplayMode'
import { useHotkeys } from '@mantine/hooks'
import { toPct } from '../../utils/numbers'

export default function Problems() {
  // State
  const zoomMode = useStore($userboardZoomMode)
  const displayMode = useStore($userboardDisplayMode)
  const isShowBigPreviews = displayMode === 'Previews'

  // Data
  const { isLoading, data: stats } = useAPIData({
    fetch: () => API.getProblemsStats(),
    deps: [],
  })

  const data = _.map(stats?.problems, (data, id) => ({
    id,
    ...data,
    stageSize: `${data.stage_width} x ${data.stage_height}`,
  }))

  // Controls
  const toggleZoomMode = () => {
    $userboardZoomMode.set(zoomMode === 'Zoom' ? 'Full' : 'Zoom')
  }
  useHotkeys([['z', () => toggleZoomMode()]])

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
                size={isShowBigPreviews ? '75vmin' : 'min(20vmin, 8rem)'}
                zoomMode={zoomMode}
              />
            </Group>
          </Link>
        ),

      header: 'Preview',
      size: 140,
    },
    ...Object.fromEntries(
      [
        ['instruments', 'Instrs'],
        ['musicians', 'Musicns'],
        ['attendees', 'Attends'],
        ['pillars', 'Pillars'],
      ].map(([accessorKey, header]) => [
        accessorKey,
        {
          accessorKey,
          header,
          size: 70,
        },
      ]) as any
    ),
    score: {
      id: 'score',
      header: 'Score',
      size: 90,
      accessorKey: 'score',
      Cell: ({ cell }) => (
        <span> {formatNumber(cell.getValue<number>())} </span>
      ),
    },
    ourBest: {
      id: 'ourBest',
      header: 'loks_best',
      size: 90,
      accessorKey: 'our_best',
      Cell: ({ cell }) => (
        <span> {formatNumber(cell.getValue<any>()['loks_best'])} </span>
      ),
    },
    scoreMismatch: {
      id: 'scoreMismatch',
      header: 'score - loks_best',
      size: 90,
      accessorFn: (originalRow) =>
        (originalRow.score || 0) -
        (originalRow.our_best && originalRow.our_best['loks_best']),
    },
    estimatedMax: {
      id: 'estimatedMax',
      header: 'Approx Max',
      size: 90,
      accessorKey: 'estimated_max',
      Cell: ({ cell }) => (
        <span> {formatNumber(cell.getValue<number>())} </span>
      ),
    },
    percentOfMax: {
      id: 'percentOfMax',
      header: '% of Max',
      size: 90,
      accessorFn: (originalRow) =>
        (originalRow.score ?? 0) / (originalRow.estimated_max || 1),
      Cell: ({ cell }) => <span>{toPct(cell.getValue<number>())}</span>,
    },

    deltaWithMax: {
      id: 'deltaWithMax',
      header: 'Delta',
      size: 90,
      accessorFn: (originalRow) =>
        (originalRow.estimated_max ?? 0) - (originalRow.score ?? 0),
      Cell: (row) => (
        <span>{formatNumberExp(row.cell.getValue<number>())}</span>
      ),
    },
    stageSize: {
      id: 'stageSize',
      header: 'Stage Size',
      size: 90,
      accessorFn: (originalRow) =>
        originalRow.stage_width &&
        `${originalRow.stage_width} x ${originalRow.stage_height}`,
    },
    tastesAndPotential: {
      id: 'tastesAndPotential',
      header: 'Tastes and Available Gains',
      size: 140,
      Cell: ({ row }) => (
        <Stack
          spacing="xs"
          sx={{
            '--problem-hist-size': isShowBigPreviews
              ? '12rem'
              : 'min(25vmin, 12rem)',
          }}
          w="var(--problem-hist-size)"
          h="var(--problem-hist-size)"
        >
          <Box pos="relative" w="100%" h="50%">
            <Image
              pos="absolute"
              opacity={0.2}
              sx={{
                filter: 'hue-rotate(.5turn)',
              }}
              src={AssetURL.hishogram(row.getValue('id'), 'tastes.log')}
            />
            <Image
              pos="absolute"
              src={AssetURL.hishogram(row.getValue('id'), 'tastes')}
            />
          </Box>
          <Box pos="relative" w="100%" h="50%">
            <Image
              pos="absolute"
              opacity={0.2}
              sx={{
                filter: 'hue-rotate(.5turn)',
              }}
              src={AssetURL.hishogram(row.getValue('id'), 'potential.log')}
            />
            <Image
              pos="absolute"
              src={AssetURL.hishogram(row.getValue('id'), 'potential')}
            />
          </Box>
        </Stack>
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
              value={displayMode}
              onChange={$userboardDisplayMode.set}
            />
            <SegmentedControl
              color="orange.5"
              size="sm"
              data={[
                {
                  label: '(Z)oom',
                  value: 'Zoom',
                },
                {
                  value: 'Full',
                  label: 'Full',
                },
              ]}
              value={zoomMode}
              onChange={$userboardZoomMode.set}
            />
          </Group>
        )}
        mantinePaperProps={{
          radius: 0,
          mah: '100%',
        }}
        columns={[
          columns.id,
          columns.preview,
          columns.instruments,
          columns.musicians,
          columns.attendees,
          columns.tastesAndPotential,
          columns.pillars,
          columns.stageSize,
          columns.score,
          columns.ourBest,
          columns.scoreMismatch,
          columns.estimatedMax,
          columns.percentOfMax,
          columns.deltaWithMax,
        ]}
        enableStickyHeader
        enablePagination={false}
        enableDensityToggle={false}
        enableColumnActions={false}
        enableHiding={false}
        state={{ isLoading, density: 'xs' }}
        data={data}
      />
    </Container>
  )
}
