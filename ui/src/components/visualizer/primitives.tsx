import _ from 'lodash'
import {
  Box as MantineBox,
  MantineColor,
  MantineSize,
  PaperProps,
  useMantineTheme,
} from '@mantine/core'
import React from 'react'

export type RectProps = {
  x: number
  y: number
  width: number
  height: number
  color?: MantineColor
  borderRadius?: MantineSize
} & PaperProps
export const Rect = ({
  x,
  y,
  width,
  height,
  color = 'white',
  borderRadius,
  ...props
}: RectProps) => {
  const theme = useMantineTheme()
  return (
    <MantineBox
      component="rect"
      x={x}
      y={y}
      width={width}
      height={height}
      fill={(_.get(theme.colors, color) || color) as any}
      rx={(borderRadius && theme.radius[borderRadius]) || borderRadius}
      {...props}
    />
  )
}

export type CircleProps = {
  x?: number
  y?: number
  radius?: number
  color?: MantineColor
  dimmed?: boolean
  dimmedColor?: MantineColor
  strokeWidth?: number
  strokeColor?: MantineColor
} & PaperProps &
  React.DOMAttributes<SVGCircleElement>
export const Circle = ({
  x,
  y,
  radius,
  strokeWidth = 0,
  color = 'white',
  dimmed = false,
  dimmedColor = '',
  strokeColor = 'black',
  ...props
}: CircleProps) => {
  const theme = useMantineTheme()
  const _color = (_.get(theme.colors, color) || color) as any
  const _dimmedColor = (_.get(theme.colors, dimmedColor) || dimmedColor) as any
  return (
    <MantineBox
      component="circle"
      cx={x}
      cy={y}
      r={radius}
      fill={(dimmed && _dimmedColor) || _color}
      opacity={dimmed ? 0.25 : 1}
      stroke={strokeColor}
      strokeWidth={strokeWidth}
      {...props}
    />
  )
}
