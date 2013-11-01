//#extension GL_ARB_gpu_shader5 : enable
uniform sampler3D myTexture3D;
uniform highp mat4 qt_ModelViewMatrix; // = MV
uniform highp mat4 qt_ModelViewProjectionMatrix; // = MV
uniform highp mat4 qt_ProjectionMatrix; // = MV
uniform highp float multiplier;
uniform highp bool useSquareRootDensity;
uniform highp vec3 eyePosition;

varying highp vec4 entryPoint; // = EntryPoint
varying highp vec4 entryPointTexCoord; // = EntryPoint

void main(void)
{
    float stepSize = 0.005;
    mat4 inverseMatrix = inverse(qt_ModelViewMatrix);
    vec4 eyeCoord = inverseMatrix[3];
    vec3 eye = eyeCoord.xyz;
//    vec3 eye = eyePosition;
    vec3 exitPoint = eye;
    vec3 direction = exitPoint - entryPoint;
    direction *= -1;
    float directionLength = length(direction);
    vec3 deltaDir = normalize(direction) * stepSize;
    float deltaDirLength = length(deltaDir);
    vec4 startPoint = entryPointTexCoord + vec4(0.5, 0.5, 0.5, 0.0); // TODO: Remove this after implementing proper texture coordinates
    vec3 voxelCoord = startPoint;
    float colorAcummulated = 0.0;
    float redAcummulated = 0.0;
    for(int i = 0; i < 1.732 / stepSize; i++) { // 1.732 = cube diagonal
        voxelCoord += deltaDir;
        vec3 voxelValue = multiplier * texture3D(myTexture3D, voxelCoord);
        if(useSquareRootDensity) {
            voxelValue.x = sqrt(voxelValue.x);
        }
        colorAcummulated += voxelValue.x * stepSize;
        if(voxelValue.x < 0.3) {
            redAcummulated += voxelValue.x * voxelValue.x * stepSize * 50;
        }
        if(voxelCoord.x > 1.0 || voxelCoord.y > 1.0 || voxelCoord.z > 1.0
                || voxelCoord.x < 0.0 || voxelCoord.y < 0.0 || voxelCoord.z < 0.0) {
            break;
        }
    }
    colorAcummulated = clamp(colorAcummulated, 0, 1);
    redAcummulated = clamp(redAcummulated, 0, 1);
    gl_FragColor = vec4(1.0, 1.0, 1.0, colorAcummulated);
//    gl_FragColor = vec4(0.0, gl_FragDepth, 1.0, 1.0);
//    gl_FragColor = vec4(normalize(eyePosition), 1.0);
//            - redAcummulated * vec4(1.0, 1.0, 0.0, 0.0);

    // Testing:
    //    gl_FragColor = vec4(normalize(eye).xyz, 1.0);
    //    gl_FragColor = vec4(normalize(direction).xyz, 1.0);
    //    gl_FragColor = vec4(normalize(eyeCoord).xyz, 1.0);
    //    gl_FragColor = vec4(deltaDir.xyz * 100.0, 1.0);
    //    gl_FragColor = vec4(traversedLength, 0.0, 0.0, 1.0);
//        gl_FragColor = vec4(texture2D(qt_Texture0, gl_FragCoord.xy / vec2(1000, 800)));

}
