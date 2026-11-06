#version 430 core
in float isTeamA;
out vec4 fragColor;

void main() {
    if(isTeamA < 0)
    fragColor = vec4(0.75, 0.5, 0.5, 1.f);
    else
    fragColor = vec4(0.5, 0.5, 0.75, 1.f);
}
