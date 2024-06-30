import { NavLink } from 'react-router-dom'
import { Box, Group, GroupProps, Text, Title } from '@mantine/core'

export default function HeaderBar({ children, ...props }: GroupProps) {
  return (
    <Group
      position="apart"
      spacing={0}
      c="white"
      bg="yellow.7"
      p="sm"
      sx={{
        '& a': {
          color: 'white',
          textDecoration: 'none',
        },
      }}
      {...props}
    >
      <NavLink to="/">
        <Title order={2} w={150}>
          ICFPC 2024
        </Title>
      </NavLink>
      <Group sx={{ flexGrow: 1 }} position="center">
        <NavLink to="/simulator">
          {({ isActive }) => (
            <Text fw="bolder" td={isActive ? 'underline' : 'none'}>
              3D Simulator
            </Text>
          )}
        </NavLink>
        <NavLink to="/interpreter">
          {({ isActive }) => (
            <Text fw="bolder" td={isActive ? 'underline' : 'none'}>
              Interpreter
            </Text>
          )}
        </NavLink>
      </Group>
      <Box maw={150} sx={{ flexGrow: 0.125 }} />
    </Group>
  )
}
