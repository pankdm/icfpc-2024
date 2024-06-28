import { Circle, CircleProps } from './primitives.3d'

export const Pillar = ({
  radius = 5,
  color = '#e72626',
  ...props
}: CircleProps) => <Circle radius={radius} color={color} {...props} />

export const Attendee = ({
  radius = 5,
  color = 'white',
  ...props
}: CircleProps) => <Circle radius={radius} color={color} {...props} />

export const Musician = ({
  radius = 5,
  color = 'gold',
  ...props
}: CircleProps) => <Circle radius={radius} color={color} {...props} />
