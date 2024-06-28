import React from 'react'
import ReactDOM from 'react-dom/client'
import App from './App.tsx'
import { BrowserRouter, HashRouter } from 'react-router-dom'
import { MantineProvider } from '@mantine/core'
import './index.css'

const RouterComponent =
  import.meta.env.MODE === 'production' ? HashRouter : BrowserRouter

ReactDOM.createRoot(document.getElementById('root') as HTMLElement).render(
  <React.StrictMode>
    <MantineProvider
      withGlobalStyles
      withNormalizeCSS
      theme={{
        colors: {},
        primaryColor: 'orange',
      }}
    >
      <RouterComponent>
        <App />
      </RouterComponent>
    </MantineProvider>
  </React.StrictMode>
)
