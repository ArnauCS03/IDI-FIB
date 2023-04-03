#version 330 core

in vec4 fcolor;
out vec4 FragColor;

void main() {
    
    FragColor = fcolor;   //vec4(1., 0., 0., 1); 

    /*
    if (gl_FragCoord.x < 354. && gl_FragCoord.y < 354.) {
        FragColor = vec4(1, 1, 0, 1);
    }
    else if (gl_FragCoord.x > 354. && gl_FragCoord.y < 354.) {
        FragColor = vec4(0, 1, 0, 1);
    }
    else if (gl_FragCoord.x > 354. && gl_FragCoord.y > 354.) {
        FragColor = vec4(0, 0, 1, 1);
    }

    if (int(gl_FragCoord.y)%25 < 10) discard;
    */
}

