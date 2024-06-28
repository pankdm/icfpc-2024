import { PropsWithChildren } from 'react'
import { Helmet } from 'react-helmet'
import { AppShell } from '@mantine/core'
import { Outlet } from 'react-router-dom'
import HeaderBar from './HeaderBar'
import config from '../config'

export default function AppLayout({ children, ...props }: PropsWithChildren) {
  return (
    <AppShell
      header={<HeaderBar />}
      styles={() => ({
        root: {
          height: '100vh',
          display: 'flex',
          flexDirection: 'column',
        },
        body: {
          flex: 1,
          overflow: 'auto',
        },
        main: {
          padding: 0,
          minHeight: 'unset',
          flex: 1,
        },
      })}
      {...props}
    >
      <Helmet>
        <title>{config.HTML_TITLE}</title>
      </Helmet>
      <Outlet />
    </AppShell>
  )
}
