import { Box, MantineColor, MantineSize, Space, Stack, StackProps } from '@mantine/core'
import Circle from './Circle.js'

const circleSize = '.13em'
const circlesTopOffset = '.297em'
const interCircleInterval = '.089em'
const circleShadow = (color: MantineColor) => `0 0 .2em .15em ${color}`

const TrafficLightCircle = ({
  color,
}: {
  size: MantineSize
  color?: MantineColor | boolean
}) => (
  <Box w={circleSize} h={circleSize} pos="relative">
    <Circle
      pos="absolute"
      size="100%"
      shadow={color && (circleShadow(color as any) as any)}
      color={color ? (color as any) : 'rgb(0,0,0,0.25)'}
      sx={{
        flexShrink: 0,
        transition: 'all .3s',
        zIndex: color ? 10 : 0,
      }}
    />
  </Box>
)

export default function TrafficLight({
  size = '5rem',
  red = false,
  yellow = false,
  green = false,
  ...props
}: {
  size?: MantineSize,
  red?: boolean,
  yellow?: boolean,
  green?: boolean,
} & StackProps) {
  return (
    <Stack
      {...props}
      justify="flex-start"
      bg="url('/traffic_light.png')"
      bgr="no-repeat"
      align="center"
      spacing={0}
      fz={size}
      bgsz="100%"
      w={size}
      h={size}
    >
      <Space h={circlesTopOffset} />
      <TrafficLightCircle size={circleSize} color={red && '#f00'} />
      <Space h={interCircleInterval} />
      <TrafficLightCircle size={circleSize} color={yellow && '#f1de0a'} />
      <Space h={interCircleInterval} />
      <TrafficLightCircle size={circleSize} color={green && '#1ef144'} />
    </Stack>
  )
}
