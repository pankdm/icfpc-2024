import { Switch } from '@mantine/core'
import { SwitchProps } from '@mantine/core'
import { persistentAtom } from '@nanostores/persistent'
import { useStore } from '@nanostores/react'
import { Atom, WritableAtom } from 'nanostores'
import React from 'react'

export const $warpDecodedResponse = persistentAtom<string>(
  'icfpc-2024:warpDeodedResponse',
  'n'
)

export const Toggle: React.FC<
  { atom: Atom | WritableAtom } & Omit<SwitchProps, 'checked' | 'onChange'>
> = ({ atom, ...switchProps }) => {
  const warpDecodedResponse = useStore($warpDecodedResponse)
  return (
    <Switch
      {...switchProps}
      checked={warpDecodedResponse === 'y'}
      onChange={(ev) => $warpDecodedResponse.set(ev.target.checked ? 'y' : 'n')}
    />
  )
}
