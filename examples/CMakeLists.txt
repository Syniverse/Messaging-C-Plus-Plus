project(scgapi-examples)

if (WIN32)
	set(REQUIRED_LIBRARIES 
		debug ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG} optimized ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}
	)
endif()

add_executable(list_senderid_types list_senderid_types.cpp)
target_link_libraries(list_senderid_types
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)


add_executable(create_contact create_contact.cpp)
target_link_libraries(create_contact
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)


add_executable(list_messages_from_mrq list_messages_from_mrq.cpp)
target_link_libraries(list_messages_from_mrq
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)


add_executable(list_senderid list_senderid.cpp)
target_link_libraries(list_senderid
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)


add_executable(send_sms send_sms.cpp)
target_link_libraries(send_sms
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)


add_executable(send_sms_to_grp send_sms_to_grp.cpp)
target_link_libraries(send_sms_to_grp
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)

add_executable(send_sms_to_grp_async send_sms_to_grp_async.cpp)
target_link_libraries(send_sms_to_grp_async
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)

add_executable(send_mms send_mms.cpp)
target_link_libraries(send_mms
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)

add_executable(check_message_request_state check_message_request_state.cpp)
target_link_libraries(check_message_request_state
    restc-cpp
    scgapi
	${REQUIRED_LIBRARIES}
)

