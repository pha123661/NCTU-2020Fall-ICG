#version 430

in vec2 uv;
in vec3 worldNormal;
in vec3 worldPos;

uniform vec3 worldLight, worldCam, Ka, Kd, Ks, La, Ld, Ls;
uniform float gloss;
uniform sampler2D texture;

out vec4 color;

void main()
{
    vec3 to_light, to_cam, reflection, tmp, ambient, diffuse, specular;
    to_light = normalize(worldLight - worldPos);
    to_cam = normalize(worldCam - worldPos);
    reflection = 2.0f * dot(worldNormal, to_light) * worldNormal - to_light;
    tmp = texture2D(texture, uv).rgb;

    float cos1, cos2;
    cos1 = max(0.0f, dot(to_light, worldNormal)); // angle between (l, n)
    cos2 = max(0.0f, dot(to_cam, reflection));    // angle between (v, r)

    ambient = La * Ka * tmp;
    diffuse = Ld * Kd * tmp * cos1;
    specular = Ls * Ks * pow(cos2, gloss);
    color = vec4(ambient + diffuse + specular, 1.0f);
}