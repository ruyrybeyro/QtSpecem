#include "MacOSAudioPermission.h"

#include <QDebug>
#include <qglobal.h>

#ifdef Q_OS_MAC
#import <AVFoundation/AVFoundation.h>
#import <dispatch/dispatch.h>
#include <QTimer>
#include <QApplication>
#endif

bool requestMacOSAudioPermission()
{
#ifdef Q_OS_MAC
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
    
    if (authStatus == AVAuthorizationStatusAuthorized) {
        // Already authorized
        return true;
    }
    
    if (authStatus == AVAuthorizationStatusDenied || authStatus == AVAuthorizationStatusRestricted) {
        // Permission has been denied or restricted
        qWarning() << "Audio permission denied or restricted";
        return false;
    }
    
    if (authStatus == AVAuthorizationStatusNotDetermined) {
        // Permission not determined yet, so we need to request it
        __block bool permissionGranted = false;
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        
        // Process events while waiting for permission to prevent UI freezing
        QTimer timer;
        timer.setInterval(100);
        timer.start();
        
        QObject::connect(&timer, &QTimer::timeout, []() {
            QApplication::processEvents();
        });
        
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeAudio completionHandler:^(BOOL granted) {
            permissionGranted = granted;
            dispatch_semaphore_signal(semaphore);
        }];
        
        // Wait for user response with a reasonable timeout
        const dispatch_time_t timeout = dispatch_time(DISPATCH_TIME_NOW, 30 * NSEC_PER_SEC);
        const long result = dispatch_semaphore_wait(semaphore, timeout);
        
        timer.stop();
        
        if (result != 0) {
            qWarning() << "Timeout waiting for audio permission";
            return false;
        }
        
        return permissionGranted;
    }
    
    return false;
#else
    // Not on macOS, no permission needed
    return true;
#endif
}

