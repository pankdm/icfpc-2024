import _ from 'lodash'
import { persistentAtom } from '@nanostores/persistent'

export const $userboardDisplayMode = persistentAtom<string>(
  'icfpc-2023:userboard_display_mode',
  'Table'
)

export const $userboardZoomMode = persistentAtom<string>(
  'icfpc-2023:userboard_zoom_mode',
  'Zoom'
)
