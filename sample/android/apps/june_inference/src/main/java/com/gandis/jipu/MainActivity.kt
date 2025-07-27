package com.gandis.jipu



import android.os.Bundle
import com.google.androidgamesdk.GameActivity
import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.provider.Settings
import android.util.Log
import androidx.appcompat.app.AlertDialog
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

class MainActivity : GameActivity() {
    private val CAMERA_PERMISSION_REQUEST_CODE = 100

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 권한 확인 및 요청
        checkAndRequestCameraPermission()
    }

    private fun checkAndRequestCameraPermission() {
        when {
            ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.CAMERA
            ) == PackageManager.PERMISSION_GRANTED -> {
                // 권한이 이미 허용됨
                Log.i("Camera", "Camera permission already granted")
                initializeNativeCamera()
            }
            ActivityCompat.shouldShowRequestPermissionRationale(
                this,
                Manifest.permission.CAMERA
            ) -> {
                // 권한 요청 이유 설명
                showPermissionRationaleDialog()
            }
            else -> {
                // 직접 권한 요청
                requestCameraPermission()
            }
        }
    }

    private fun showPermissionRationaleDialog() {
        AlertDialog.Builder(this)
            .setTitle("카메라 권한 필요")
            .setMessage("앱에서 카메라를 사용하기 위해 권한이 필요합니다.")
            .setPositiveButton("허용") { _, _ ->
                requestCameraPermission()
            }
            .setNegativeButton("취소") { _, _ ->
                Log.w("Camera", "Camera permission denied by user")
                // 필요시 권한 없이 진행하거나 앱 종료
                showPermissionDeniedDialog()
            }
            .setCancelable(false)
            .show()
    }

    private fun requestCameraPermission() {
        Log.i("Camera", "Requesting camera permission")
        ActivityCompat.requestPermissions(
            this,
            arrayOf(Manifest.permission.CAMERA),
            CAMERA_PERMISSION_REQUEST_CODE
        )
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)

        when (requestCode) {
            CAMERA_PERMISSION_REQUEST_CODE -> {
                if (grantResults.isNotEmpty() &&
                    grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // 권한 허용됨
                    Log.i("Camera", "Camera permission granted by user")
                    initializeNativeCamera()
                } else {
                    // 권한 거부됨
                    Log.e("Camera", "Camera permission denied by user")

                    // "다시 묻지 않음"을 선택했는지 확인
                    if (!ActivityCompat.shouldShowRequestPermissionRationale(
                            this,
                            Manifest.permission.CAMERA
                        )) {
                        // "다시 묻지 않음" 선택됨 - 설정으로 안내
                        showPermissionPermanentlyDeniedDialog()
                    } else {
                        // 일반적인 거부 - 다시 요청 가능
                        showPermissionDeniedDialog()
                    }
                }
            }
        }
    }

    private fun showPermissionDeniedDialog() {
        AlertDialog.Builder(this)
            .setTitle("카메라 권한 거부됨")
            .setMessage("카메라 기능을 사용하려면 권한이 필요합니다. 다시 시도하시겠습니까?")
            .setPositiveButton("다시 시도") { _, _ ->
                checkAndRequestCameraPermission()
            }
            .setNegativeButton("나중에") { _, _ ->
                Log.w("Camera", "User chose to grant permission later")
                // 권한 없이 진행하거나 기능 제한
            }
            .setCancelable(false)
            .show()
    }

    private fun showPermissionPermanentlyDeniedDialog() {
        AlertDialog.Builder(this)
            .setTitle("권한 설정 필요")
            .setMessage("카메라 권한이 영구적으로 거부되었습니다. 설정에서 수동으로 권한을 허용해주세요.")
            .setPositiveButton("설정으로 이동") { _, _ ->
                openAppSettings()
            }
            .setNegativeButton("취소") { _, _ ->
                Log.w("Camera", "User declined to open settings")
                // 권한 없이 진행하거나 앱 종료
            }
            .setCancelable(false)
            .show()
    }

    private fun openAppSettings() {
        try {
            val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
                data = Uri.fromParts("package", packageName, null)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            startActivity(intent)
        } catch (e: Exception) {
            Log.e("Camera", "Failed to open app settings", e)
            // 대체 방법으로 일반 설정 열기
            val intent = Intent(Settings.ACTION_SETTINGS)
            startActivity(intent)
        }
    }

    override fun onResume() {
        super.onResume()

        // 설정에서 돌아왔을 때 권한 상태 재확인
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.CAMERA
            ) == PackageManager.PERMISSION_GRANTED) {
            Log.i("Camera", "Camera permission granted after returning from settings")
//            initializeNativeCamera()
        }
    }

    private fun initializeNativeCamera() {
        Log.i("Camera", "Initializing native camera...")
        // 여기서 네이티브 카메라 초기화 함수 호출
        try {
            nativeInitializeCamera()
        } catch (e: Exception) {
            Log.e("Camera", "Failed to initialize native camera", e)
        }
    }

    // 네이티브 함수 선언
    private external fun nativeInitializeCamera()

    companion object {
        init {
            System.loadLibrary("june_inference")
        }
    }
}
