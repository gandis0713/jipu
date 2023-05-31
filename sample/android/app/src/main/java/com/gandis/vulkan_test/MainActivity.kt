package com.gandis.vulkan_test

import android.os.Bundle
import com.google.androidgamesdk.GameActivity

class MainActivity : GameActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    /**
     * A native method that is implemented by the 'vulkan_test' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
    external fun stringFromJNI2(): String

    companion object {
        // Used to load the 'vulkan_test' library on application startup.
        init {
            System.loadLibrary("vulkan_test")
        }
    }
}

//import androidx.appcompat.app.AppCompatActivity
//import android.os.Bundle
//import com.gandis.vulkan_test.databinding.ActivityMainBinding
//
//class MainActivity : AppCompatActivity() {
//
//    private lateinit var binding: ActivityMainBinding
//
//    override fun onCreate(savedInstanceState: Bundle?) {
//        super.onCreate(savedInstanceState)
//
//        binding = ActivityMainBinding.inflate(layoutInflater)
//        setContentView(binding.root)
//
//        // Example of a call to a native method
//        binding.sampleText.text = stringFromJNI()
//        binding.sampleText.text = stringFromJNI2()
//    }
//
//    /**
//     * A native method that is implemented by the 'vulkan_test' native library,
//     * which is packaged with this application.
//     */
//    external fun stringFromJNI(): String
//    external fun stringFromJNI2(): String
//
//    companion object {
//        // Used to load the 'vulkan_test' library on application startup.
//        init {
//            System.loadLibrary("vulkan_test")
//        }
//    }
//}