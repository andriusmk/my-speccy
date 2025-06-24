//      Copyright © 2025  Andrius Mazeikis
//
//      This program is free software: you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation, either version 3 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

import MetalKit

class Renderer: NSObject, MTKViewDelegate {
    
    let commandQueue: MTLCommandQueue
    let device: MTLDevice
    let pipelineState: MTLRenderPipelineState
    let vertexBuffer: MTLBuffer
    let width: Int
    let height: Int
    let bytesPerRow: Int
    let view: MTKView
    var availableTasks: [DrawTask] = []
    var taskToDraw: DrawTask?
    var viewTransform: [Float32] = [ 1.0, 0.0, 0.0, 1.0]

    class DrawTask {
        let texture: MTLTexture
        let viewTransform: MTLBuffer
        
        init(_ device: MTLDevice, textureDescriptor: MTLTextureDescriptor) {
            texture = device.makeTexture(descriptor: textureDescriptor)!
            viewTransform = device.makeBuffer(length: 16, options: .storageModeManaged)!
        }
    }

    class func create(for view: MTKView, width: Int, height: Int, bytesPerRow: Int) -> Renderer {
        let renderer = Renderer(for: view, width: width, height: height, bytesPerRow: bytesPerRow)
        view.delegate = renderer
        renderer.updateSize(width: view.frame.width, height: view.frame.height)

        return renderer
    }

    init(for view: MTKView, width: Int, height: Int, bytesPerRow: Int) {
        
        self.view = view
        self.width = width
        self.height = height
        self.bytesPerRow = bytesPerRow
        
        device = MTLCreateSystemDefaultDevice()!
        view.device = device
        view.colorPixelFormat = .bgra8Unorm
        view.preferredFramesPerSecond = 50
        view.isPaused = true
        view.enableSetNeedsDisplay = true
        commandQueue = device.makeCommandQueue()!
                
        let pipelineDescriptor = MTLRenderPipelineDescriptor()
        let library = device.makeDefaultLibrary()!

        pipelineDescriptor.vertexFunction = library.makeFunction(name: "vertex_func")
        pipelineDescriptor.fragmentFunction = library.makeFunction(name: "fragment_func")

        pipelineDescriptor.vertexDescriptor!.attributes[0].bufferIndex = 0
        pipelineDescriptor.vertexDescriptor!.attributes[0].format = .float2
        pipelineDescriptor.vertexDescriptor!.attributes[0].offset = 0
        pipelineDescriptor.vertexDescriptor!.attributes[1].bufferIndex = 0
        pipelineDescriptor.vertexDescriptor!.attributes[1].format = .float2
        pipelineDescriptor.vertexDescriptor!.attributes[1].offset = 8
        pipelineDescriptor.vertexDescriptor!.layouts[0].stride = 16
        pipelineDescriptor.colorAttachments[0].pixelFormat = .bgra8Unorm

        pipelineState = try! device.makeRenderPipelineState(descriptor: pipelineDescriptor)

        let vertices: [Float32] = [
            -1, -1, 0, 1,
            -1,  1, 0, 0,
             1, -1, 1, 1,
             1,  1, 1, 0
        ]
        
        let verticesSize = MemoryLayout<Float32>.stride * vertices.count
        
        let sharedVertexBuffer = device.makeBuffer(bytes: vertices, length: verticesSize)!
        vertexBuffer = device.makeBuffer(length: verticesSize, options: .storageModePrivate)!
        let commandBuffer = commandQueue.makeCommandBuffer()!
        let encoder = commandBuffer.makeBlitCommandEncoder()!
        encoder.copy(from: sharedVertexBuffer, sourceOffset: 0, to: vertexBuffer, destinationOffset: 0, size: verticesSize)
        encoder.endEncoding()
        commandBuffer.commit()
        
        let textureDescriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: view.colorPixelFormat, width: width, height: height, mipmapped: false)
        
        textureDescriptor.storageMode = .managed
        textureDescriptor.usage = .shaderRead
        
        for _ in 0..<3 {
            availableTasks.append(DrawTask(device, textureDescriptor: textureDescriptor))
        }
    }
    
    func updateImage(bytes: UnsafeMutableRawPointer) {
        
        guard let task = availableTasks.popLast() else { return }
        task.texture.replace(region: MTLRegion(origin: MTLOriginMake(0, 0, 0), size: MTLSize(width: width, height: height, depth: 1)), mipmapLevel: 0, withBytes: bytes, bytesPerRow: bytesPerRow)
        
        if let ttd = taskToDraw {
            availableTasks.append(ttd)
        }
        taskToDraw = task
        view.setNeedsDisplay(view.frame)
    }
    
    func updateSize(width: CGFloat, height: CGFloat) {
        let aspectRatio = width / height / (4.0 / 3.0)
        if aspectRatio < 1.0 {
            viewTransform[0] = Float32(1.0 / aspectRatio)
            viewTransform[3] = 1.0
        } else {
            viewTransform[3] = Float32(aspectRatio)
            viewTransform[0] = 1.0
        }
    }
    
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        updateSize(width: size.width, height: size.height)
    }
    
    func draw(in view: MTKView) {

        guard let task = taskToDraw,
              let renderPassDescriptor = view.currentRenderPassDescriptor
        else {
            return
        }
       
        viewTransform.withUnsafeBytes {
            $0.copyBytes(to: UnsafeMutableRawBufferPointer(start: task.viewTransform.contents(), count: task.viewTransform.length))
            return
        }
        task.viewTransform.didModifyRange(0..<task.viewTransform.length)

        renderPassDescriptor.colorAttachments[0].loadAction = .dontCare
        
        if  let drawable = view.currentDrawable,
            let commandBuffer = commandQueue.makeCommandBuffer() {
                if let encoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) {
                    taskToDraw = nil

                    encoder.setRenderPipelineState(pipelineState)
                    encoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
                    encoder.setVertexBuffer(task.viewTransform, offset: 0, index: 1)
                    encoder.setFragmentTexture(task.texture, index: 0)
                    encoder.drawPrimitives(type: .triangleStrip, vertexStart: 0, vertexCount: 4)
                    encoder.endEncoding()
                }

                commandBuffer.present(drawable)

                commandBuffer.addCompletedHandler { _ in
                    self.availableTasks.append(task)
                }
                commandBuffer.commit()
        }
    }
}
