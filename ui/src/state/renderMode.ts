import _ from 'lodash'
import { persistentAtom } from '@nanostores/persistent'
import { computed } from 'nanostores'

export const $renderMode = persistentAtom<string>(
  'icfpc-2023:render_mode',
  'svg'
)

export const $zoomMode = persistentAtom<string>('icfpc-2023:zoom_mode', 'Full')

const $_previewInstruments = persistentAtom<string>(
  'icfpc-2023:preview_instruments',
  'false'
)
export const $previewInstruments = computed(
  $_previewInstruments,
  (v) => v === 'true'
)
export const togglePreviewInstruments = () =>
  $_previewInstruments.set($previewInstruments.get() ? 'false' : 'true')

export const $previewInstrumentsMode = persistentAtom<
  'musicians_only' | 'linear' | 'log'
>('icfpc-2023:preview_instruments:mode', 'musicians_only')
