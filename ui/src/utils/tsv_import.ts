function parseLabels(cells: string[][]) {
  let labelToLocation: Record<string, [number, number]> = {}
  for (let y = 0; y < cells.length; y++) {
    let row = cells[y]
    for (let x = 0; x < row.length; x++) {
      let value = row[x]
      if (value === '<' || value === '>') continue
      if (value.includes('<')) {
        let label = value.split('<')[1]
        labelToLocation[label] = [x, y]
      }
    }
  }

  let overwrites: Record<string, string> = {}
  for (let y = 0; y < cells.length; y++) {
    let row = cells[y]
    for (let x = 0; x < row.length; x++) {
      let value = row[x]
      if (value.includes('@')) {
        if (!value.includes('>')) {
          throw new Error(`Error: @-value '${value}' without '>' label!`)
        }
        let label = value.split('>')[1]
        let [x0, y0] = labelToLocation[label]

        let dx = x - x0
        let dy = y - y0

        overwrites[`${x - 1},${y}`] = dx.toString()
        overwrites[`${x + 1},${y}`] = dy.toString()
      }
    }
  }

  return overwrites
}

function postprocessValue(value: string) {
  if (['*', '=', '<', '>'].includes(value)) {
    return value
  } else if (value === 'add') {
    return '+'
  } else if (value === 'eq') {
    return '='
  } else if (value === 'neq') {
    return '#'
  } else if (value.includes('*')) {
    return '.'
  } else if (value.includes('<')) {
    return postprocessValue(value.split('<')[0])
  } else if (value.includes('>')) {
    return postprocessValue(value.split('>')[0])
  } else if (value.includes('=')) {
    return value.split('=')[1]
  } else if (value === '') {
    return '.'
  } else {
    return value
  }
}

function postprocessInput(
  cells: string[][],
  overwrites: Record<string, string>
) {
  let data = []
  for (let y = 0; y < cells.length; y++) {
    let row = cells[y]
    let values = []
    for (let x = 0; x < row.length; x++) {
      let key = `${x},${y}`
      if (overwrites.hasOwnProperty(key)) {
        values.push(overwrites[key])
        continue
      }
      let after = postprocessValue(row[x])
      values.push(after)
    }
    data.push(values)
  }
  return data
}

export function tsvToBoard(tsvInput: string) {
  let cells = []
  let rows = tsvInput.split('\n')
  for (let row of rows) {
    let values = row.split('\t').map((value) => value.trim())
    cells.push(values)
  }

  let overwrites = parseLabels(cells)
  let data = postprocessInput(cells, overwrites)
  return data.map((row) => row.join(' ')).join('\n')
}
