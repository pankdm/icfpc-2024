import { Box, Image, MantineSize, Paper, PaperProps } from '@mantine/core'
import { API_URL } from '../api'
import { ProblemStats } from '../api/types'
import { getZoomedScaleOffset } from '../utils/camera'
import { toPct } from '../utils/numbers'

const ProblemPreview = ({
  size,
  problemId,
  problemStats,
  zoomMode,
  ...props
}: {
  size: MantineSize
  problemId: number | string
  problemStats?: ProblemStats | null
  zoomMode?: string
} & PaperProps) => {
  const zoom =
    problemStats && zoomMode === 'Zoom'
      ? getZoomedScaleOffset(problemStats)
      : null
  return (
    <Paper w={size} h={size} bg="gray.8" {...props}>
      <Box w={'100%'} h={'100%'} sx={{ overflow: 'hidden' }} pos="relative">
        <Image
          src={`${API_URL}/problems/${problemId}/preview`}
          pos="absolute"
          {...(zoom && {
            sx: {
              transform: `
                scale(${toPct(zoom.scale)})
                translate(${toPct(-zoom.offset.x)}, ${toPct(zoom.offset.y)})
              `,
            },
          })}
        />
      </Box>
    </Paper>
  )
}

export default ProblemPreview
