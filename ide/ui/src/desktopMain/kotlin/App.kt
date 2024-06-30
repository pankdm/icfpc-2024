import androidx.compose.desktop.ui.tooling.preview.Preview
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.material.Button
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import dev.icfpc.icfpc2024.core.Board
import io.github.vinceglb.filekit.compose.rememberFilePickerLauncher
import io.github.vinceglb.filekit.core.PickerMode
import kotlinx.coroutines.launch

@Composable
@Preview
fun App() {
    MaterialTheme {
        var board: Board? by remember { mutableStateOf(null) }
        val scope = rememberCoroutineScope()

        val launcher = rememberFilePickerLauncher(
            mode = PickerMode.Single,
        ) { file ->
            scope.launch {
                file?.readBytes()?.let(::String)?.let { content ->
                    board = Board.Builder
                        .fromString(content)
                        .withA(0)
                        .withB(0)
                        .build()
                }
            }
        }

        Column(Modifier.fillMaxWidth(), horizontalAlignment = Alignment.CenterHorizontally) {
            Button(
                onClick = { launcher.launch() },
            ) {
                Text("Open File")
            }

            board?.let { board ->
                BoardGrid(
                    modifier = Modifier.fillMaxSize(),
                    board = board,
                )
            }
        }
    }
}