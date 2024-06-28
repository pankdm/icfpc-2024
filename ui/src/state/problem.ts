import _ from 'lodash'
import { Store, atom, computed } from 'nanostores'
import { persistentAtom } from '@nanostores/persistent'
import { Problem } from '../api/types'

const $problemIdStored = persistentAtom<string>(
  'icfpc-2023:current_problem',
  '1'
)
export const $problemId = computed<number | null, Store>(
  $problemIdStored,
  (v) => (_.isEmpty(v) ? null : parseInt(v))
)
export const setProblemId = (newValue: number | string) =>
  $problemIdStored.set(`${newValue}`)
export const $problem = atom<Problem | null>(null)
