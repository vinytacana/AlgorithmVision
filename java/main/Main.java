package main;

import javax.swing.SwingUtilities;

/**
 * Ponto de entrada da aplicacao integrada Java + OpenGL (JNI).
 *
 * Duas threads cooperam:
 *  - "OpenGL-Render": executa MotorGrafico.init(), que bloqueia com o loop
 *    de renderizacao GLFW (o contexto OpenGL pertence a essa thread);
 *  - EDT (Event Dispatch Thread): executa a interface Swing, que envia
 *    comandos ao motor via JNI.
 */
public class Main {

    public static void main(String[] args) {
        MotorGrafico motor = new MotorGrafico();

        Thread glThread = new Thread(motor::init, "OpenGL-Render");
        glThread.start();

        SwingUtilities.invokeLater(() -> {
            ControlUI ui = new ControlUI(motor);
            ui.setVisible(true);
        });
    }
}
