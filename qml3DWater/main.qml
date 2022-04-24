// The reflection of the water is based on the "Projection Texture Mapping" technique.
//
// Reflection:
//    http://jimmiejohnsson84.me/pages/rendering_water.html
//    https://www.youtube.com/watch?v=GADTasvDOX4&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&index=6
import QtQuick 2.15 as QQ2

import Qt3D.Core 2.15
import Qt3D.Render 2.15
import Qt3D.Input 2.15
import Qt3D.Extras 2.15

Entity {
    id: root

    function degrees(radians) {
        return radians * 180 / Math.PI;
    }

    function radians(degrees) {
        return degrees * Math.PI / 180;
    }

    //
    // mainCamera is used to render the entire scene with perspective projection
    // and used a second time to render the geometry that is under the water surface (i.e. below the clipping plane)
    //

    Camera {
        id: mainCamera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane: 0.01
        farPlane: 10000.0

        position: Qt.vector3d(5, 126, 152)
        viewCenter: Qt.vector3d(3.8, 8.8, 14.0)
        upVector:   Qt.vector3d(0.0, 1.0, 0.0)

//        onPositionChanged: console.log("  mainCamera.position=", mainCamera.position)
//        onViewCenterChanged: console.log("mainCamera.viewCenter=", mainCamera.viewCenter)
    }

    //
    // reflectionCamera mirrors the movement of mainCamera under water and is used to create the
    // reflection texture which will contain geometry that is above the water surface
    // (i.e. above the clipping plane)
    //
    // https://youtu.be/0NH9k4zTAqk?list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&t=675

    Camera {
        id: reflectionCamera

        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 60 // hack: increasing FOV allows to capture a bigger scene
        nearPlane: 0.01
        farPlane: 10000.0

        // cam height above the water
        property real cam2WaterLevelDist: orbitCamController.camera.position.y - waterEntity.pos.y;

        // move cam under water
        position: Qt.vector3d(orbitCamController.camera.position.x,
                              orbitCamController.camera.position.y - (2.0 * cam2WaterLevelDist),
                              orbitCamController.camera.position.z)
        // invert pitch
        viewCenter: Qt.vector3d(orbitCamController.camera.viewCenter.x,
                                orbitCamController.camera.viewCenter.y * -1,
                                orbitCamController.camera.viewCenter.z)

        upVector:   Qt.vector3d(0.0, 1.0, 0.0)

//        onPositionChanged: {
//            console.log("        waterEntity.pos.y=", waterEntity.pos.y);
//            console.log("       cam2WaterLevelDist=", reflectionCamera.cam2WaterLevelDist);
//            console.log("   reflectionCam.position=", reflectionCamera.position);
//        }
    }

    //
    // orthoCamera is used to render offscreen textures to small viewports
    //

    Camera {
        id: orthoCamera
        left: -1
        right: 1
        bottom: -1
        top: 1
        projectionType: CameraLens.OrthographicProjection
        position: Qt.vector3d(0, 0, 2)
        viewCenter: Qt.vector3d(0, 0, 0)
    }

    OrbitCameraController {
        id: orbitCamController
        camera: mainCamera; linearSpeed: 500
    }

    components: [
        RenderSettings {
            pickingSettings.pickMethod: PickingSettings.TrianglePicking
            activeFrameGraph: frameGraph
        },

        InputSettings { } // event Source will be set by the Qt3DQuickWindow,
    ]

    //
    // Layers:
    //   lightsLayer is for the lights on the scene
    //   objectsLayer is for the ground and terrain
    //   waterLayer is for the water surface
    //   clippingLayer1 is for the offscreen reflection texture
    //   clippingLayer2 is for the offscreen refraction texture

    Layer { id: lightsLayer }
    Layer { id: objectsLayer }
    Layer { id: waterLayer }
    Layer { id: clippingLayer1 }
    Layer { id: clippingLayer2 }

    //
    // Lights
    //       

    // camera light
    Entity {
        components: [ lightsLayer, camLight, camLightTransform ]

        PointLight { id: camLight; intensity: 0.5 }
        Transform { id: camLightTransform; translation: orbitCamController.camera.position }
    }

    // sun light
    Entity {
        id: sunLightEntity
        property alias position: sunLightTransform.translation

        components: [ lightsLayer, sunLight, sunLightTransform ]

        PointLight { id: sunLight; intensity: 0.2 }
        Transform { id: sunLightTransform; translation: Qt.vector3d(0, 100, 0) }
    }

    TerrainEntity {
        id: terrainEntity
        enabled: true
        layer: objectsLayer
        pos: Qt.vector3d(0, 0, 0)
        scale: 2.0
    }

    TreeEntity {
        id: treeEntity
        layer: objectsLayer
    }

    //
    // Water (blue 2D plane)
    //

    WaterEntity {
        id: waterEntity
        enabled: true
        layer: waterLayer
        pos: Qt.vector3d(0.0, 9.0, 0.0) // 9.0
        width: 250
        height: 250

        reflectionTexture: colorAttachmentReflection                                                // reflection texture
        refractionTexture: colorAttachmentRefraction                                                // refraction texture
        refractionDepthTexture: depthAttachmentRefraction
        waterViewProjection: reflectionCamera.projectionMatrix.times(reflectionCamera.viewMatrix)   // project reflection on the water surface
        dudvMap: TextureLoader { source: "qrc:/assets/waterDUDV.png" }                              // dudv map causes distortion of the textures
        cameraPosition: orbitCamController.camera.position                                          // used for fresnel effect
        normalMap: TextureLoader { source: "qrc:/assets/waterNormalMap.png" }                       // adds specular highlights on the surface
        lightColor: Qt.vector3d(1.0, 1.0, 1.0) // white light
        lightPosition: sunLightEntity.position
    }

    //
    // Display 2D texture on the top-left to show the water reflection
    //

    TextureEntity {
        enabled: true
        layer: clippingLayer1
        pos: Qt.vector3d(0.0, 0.0, 0.0)
        width: 2
        height: 2
        offscreenTexture: colorAttachmentReflection
    }

    //
    // Display 2D texture on the top-right to show the water refraction
    //

    TextureEntity {
        enabled: true
        layer: clippingLayer2
        pos: Qt.vector3d(0.0, 0.0, 0.0)
        width: 2
        height: 2
        offscreenTexture: colorAttachmentRefraction
    }

    //
    // Offscreen framebuffer (FBO) that stores refraction
    // (scene below the horizontal clipping plane)
    //

    RenderTarget {
        id: offscrTargetRefraction
        readonly property real width: frameGraph.surface ? frameGraph.surface.width : 0
        readonly property real height: frameGraph.surface ? frameGraph.surface.height : 0

        // attachments: attach a texture image to the framebuffer attachent point
        attachments : [
            RenderTargetOutput {
                attachmentPoint: RenderTargetOutput.Color0
                texture: Texture2D {
                    id: colorAttachmentRefraction
                    width: offscrTargetRefraction.width
                    height: offscrTargetRefraction.height
                    format: Texture.RGBA8_UNorm
                }
            },

            RenderTargetOutput {
                attachmentPoint : RenderTargetOutput.Depth
                texture : Texture2D {
                    id: depthAttachmentRefraction
                    width: offscrTargetRefraction.width
                    height: offscrTargetRefraction.height
                    format: Texture.DepthFormat
                }
            }
        ]
    }

    //
    // Offscreen framebuffer (FBO) that stores reflection
    // (scene above the horizontal plane)
    //

    RenderTarget {
        id: offscrTargetReflection
        readonly property real width: frameGraph.surface ? frameGraph.surface.width : 0
        readonly property real height: frameGraph.surface ? frameGraph.surface.height : 0

        // attachments: attach a texture image to the framebuffer attachent point
        attachments : [
            RenderTargetOutput {
                attachmentPoint: RenderTargetOutput.Color0
                texture: Texture2D {
                    id: colorAttachmentReflection
                    width: offscrTargetReflection.width
                    height: offscrTargetReflection.height
                    format: Texture.RGBA8_UNorm
                    magnificationFilter: Texture.Linear
                    minificationFilter: Texture.Linear
                    wrapMode {
                        x: WrapMode.ClampToBorder
                        y: WrapMode.ClampToBorder
                    }
                    comparisonFunction: Texture.CompareLessEqual
                    comparisonMode: Texture.CompareRefToTexture
                }
            }
        ]
    }

    // RenderSurfaceSelector chooses which window and part of the window the drawing takes place but
    // it can render to a texture instead.
    //
    // This creates as many RenderViews as leafs on the tree:
    // RV0: RSS, CB, RTS, LF, RSS, CS, RPF, traverse-scenegraph-and-draw      // render scene to reflection texture
    // RV1: RSS, CB, RTS, LF, RSS, CS, RPF, traverse-scenegraph-and-draw      // render scene to refraction texture
    // RV2: RSS, VP, CB, LF, RSS, CS, traverse-scenegraph-and-draw            // render scene to full screen without water
    // RV3: RSS, VP, LF, CS, traverse-scenegraph-and-draw                     // render just the water
    // RV4: RSS, VP, LF, CS, traverse-scenegraph-and-draw                     // render reflection texture to small viewport
    // RV5: RSS, VP, LF, CS, traverse-scenegraph-and-draw                     // render refraction texture to small viewport
    RenderSurfaceSelector {
        id: frameGraph        

        // offscreen rendering: reflection texture stores the scene above the horizontal plane
        ClearBuffers {
            buffers: ClearBuffers.ColorDepthBuffer
            clearColor: "transparent" // cyan

            SortPolicy {
                sortTypes: [ SortPolicy.BackToFront ]

                RenderTargetSelector {
                    target: offscrTargetReflection

                    LayerFilter {
                        layers: [ objectsLayer, lightsLayer ]

                        RenderStateSet {
                            renderStates: [
                                // enables clipping plane for vertex shader: gl_ClipDistance[0]
                                ClipPlane { planeIndex: 0 },
                                DepthTest { depthFunction: DepthTest.Less }
                                //CullFace { mode: CullFace.NoCulling }
                            ]

                            CameraSelector {
                                camera: reflectionCamera

                                // sets gl_ClipDistance[0] to clip everything below the horizontal plane
                                RenderPassFilter {
                                    matchAny: FilterKey { name: "clippingPlane"; value: 2 }
                                }
                            }
                        }
                    }
                }
            }
        }

        // offscreen rendering: refraction texture stores the scene below the horizontal plane
        ClearBuffers {
            buffers: ClearBuffers.ColorDepthBuffer
            clearColor: "transparent" // "brown"

            RenderTargetSelector {
                target: offscrTargetRefraction

                LayerFilter {
                    layers: [ objectsLayer, lightsLayer ]

                    RenderStateSet {
                        renderStates: [
                            // enables clipping plane for vertex shader: gl_ClipDistance[0]
                            ClipPlane { planeIndex: 0 },
                            DepthTest { depthFunction: DepthTest.Less }
                        ]

                        CameraSelector {
                            camera: mainCamera

                            // sets gl_ClipDistance[0] to clip everything above the horizontal plane
                            RenderPassFilter {
                                matchAny: FilterKey { name: "clippingPlane"; value: 1 }
                            }
                        }
                    }
                }
            }
        }

        // draw the entire scene using perspective projection to a full viewport
        Viewport {
            normalizedRect: Qt.rect(0, 0, 1, 1)

            // clear the buffers then draw the ground and terrain
            ClearBuffers {
                buffers: ClearBuffers.ColorDepthBuffer
                clearColor: "white"

                LayerFilter {
                    layers: [ objectsLayer, lightsLayer ]

                    RenderStateSet {
                        renderStates: [
//                            CullFace { mode : CullFace.Back },
                            DepthTest { depthFunction: DepthTest.Less },
//                            NoDepthMask { },
                            BlendEquationArguments {
                                sourceRgb: BlendEquationArguments.SourceAlpha
                                destinationRgb: BlendEquationArguments.OneMinusSourceAlpha
                            },
                            BlendEquation { blendFunction: BlendEquation.Add }
                        ]

                        CameraSelector {
                            camera: mainCamera                            
                        }
                    }
                }
            }

            // draw the water on top of everything else
            LayerFilter {
                layers: [ waterLayer, lightsLayer ]

                CameraSelector {
                    camera: mainCamera
                }
            }
        }

        // preview reflection texture on the top-right viewport
        Viewport {
            normalizedRect: Qt.rect(0.75, 0.05, 0.20, 0.20)

            LayerFilter {
                layers: [ clippingLayer1, lightsLayer ]

                CameraSelector {
                    camera: orthoCamera
                }
            }
        }

        // preview refraction texture on the top-right viewport
        Viewport {
            normalizedRect: Qt.rect(0.75, 0.30, 0.20, 0.20)

            LayerFilter {
                layers: [ clippingLayer2, lightsLayer ]

                CameraSelector {
                    camera: orthoCamera
                }
            }
        }
    }
}

