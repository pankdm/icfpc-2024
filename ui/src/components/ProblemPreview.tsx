import { Box, Image, MantineSize, Paper, PaperProps } from '@mantine/core'
import { API_URL } from '../api'
import { ProblemStats } from '../api/types'

const ProblemPreview = ({
  size,
  problemId,
  problemStats,
  ...props
}: {
  size: MantineSize
  problemId: number | string
  problemStats?: ProblemStats | null
} & PaperProps) => {
  return (
    <Paper w={size} h={size} bg="gray.8" {...props}>
      <Box w={'100%'} h={'100%'} sx={{ overflow: 'hidden' }} pos="relative">
        <Image
          src={`${API_URL}/problems/${problemId}/preview`}
          pos="absolute"
        />
      </Box>
    </Paper>
  )
}

export default ProblemPreview
