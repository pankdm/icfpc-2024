import { MantineColor, MantineSize, Paper, PaperProps } from '@mantine/core'

export default function Circle({
  size = 'auto',
  color = '#ccc',
  ...props
}: { size: MantineSize; color: MantineColor } & PaperProps) {
  return (
    <Paper
      radius="50%"
      withBorder={false}
      bg={color}
      w={size}
      h={size}
      {...props}
    />
  )
}
