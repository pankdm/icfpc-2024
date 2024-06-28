export function formatNumber(num?: number | null) {
  const _num: number = +(num as number)
  if (Number.isNaN(_num)) return
  return _num.toLocaleString('en-US')
}
export function formatNumberExp(num?: number | null) {
  const _num: number = +(num as number)
  if (Number.isNaN(_num)) return
  return _num.toExponential(2)
}

export function toPct(num?: number | null, precision = 3) {
  const _num: number = +(num as number)
  if (Number.isNaN(_num)) return
  return `${(100 * _num).toFixed(precision)}%`
}
