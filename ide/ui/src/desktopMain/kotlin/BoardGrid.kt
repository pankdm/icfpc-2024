import androidx.compose.desktop.ui.tooling.preview.Preview
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.drawText
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.unit.dp
import dev.icfpc.icfpc2024.core.Board
import dev.icfpc.icfpc2024.core.Point


@Composable
fun BoardGrid(
    modifier: Modifier = Modifier,
    board: Board,
) {
    val allCells = remember { board.toGrid() }
    val textMeasurer = rememberTextMeasurer()

    Canvas(
        modifier = Modifier
            .size(
                width = (64 * board.width).dp,
                height = (64 * board.height).dp,
            )
            .background(color = Color.LightGray),
    ) {
        allCells.forEachIndexed { y, row ->
            row.forEachIndexed { x, s ->
                val text = textMeasurer.measure(s)
                drawText(text, topLeft = Offset(x = x * 64 + 32f, y = y * 64 + 32f))
            }
        }
    }
}

fun Board.toGrid() = (minY..maxY).map { y ->
    (minX..maxX).map { x ->
        get(Point(x, y))?.toString() ?: "."
    }
}

@Composable
@Preview
fun BoardPreview() {
    BoardGrid(
        board = Board.Builder.fromString(
            """
                . B .
                A + S
            """.trimIndent()
        ).withA(1).withB(2).build()
    )
}
