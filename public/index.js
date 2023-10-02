// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    // jQuery.ajax({
    //     type: 'get',            //Request type
    //     dataType: 'json',       //Data type - we will use JSON for almost everything
    //     url: '/endpoint1',   //The server endpoint we are connecting to
    //     data: {
    //         data1: "Value 1",
    //         data2:1234.56
    //     },
    //     success: function (data) {
    //         /*  Do something with returned object
    //             Note that what we get is an object, not a string,
    //             so we do not need to parse it on the server.
    //             JavaScript really does handle JSONs seamlessly
    //         */
    //         jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
    //         //We write the object to the console to show that the request was successful
    //         console.log(data);
    //
    //     },
    //     fail: function(error) {
    //         // Non-200 return, do something with error
    //         $('#blah').html("On page load, received error from server");
    //         console.log(error);
    //     }
    // });
    //
    // // Event listener form example , we can use this instead explicitly listening for events
    // // No redirects if possible
    // $('#someform').submit(function(e){
    //     $('#blah').html("Form has data: "+$('#entryBox').val());
    //     e.preventDefault();
    //     //Pass data to the Ajax call, so it gets passed to the server
    //     $.ajax({
    //         //Create an object for connecting to another waypoint
    //     });
    // });
    $("#viewpanelForm").submit(function(e){
      e.preventDefault();
      //console.log( $('#panelformdiv').val());
      $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/viewpanel',   //The server endpoint we are connecting to
        data: {
            filename: $('#panelformdiv').val()
        },
      //  var list = new
        success: function (data) {
          console.log(data);
            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            let arr = data.routes;
            let i = 1;
            arr.forEach(function(obj){
              $("#gpxviewpanel_tbody").append('<tr><td>'+'Route '+i+
              '</td><td>'+obj.name+'</td><td>'+obj.numPoints+
              '</td><td>'+obj.len + "m"+'</td><td>'+obj.loop+'</td></tr>');
              i++;
                });

            let arr2 = data.tracks;
            i = 1;
            arr2.forEach(function(obj){
              $("#gpxviewpanel_tbody").append('<tr><td>'+"Track" +i+
              '</td><td>'+obj.name+'</td><td>'+obj.numPoints+
              '</td><td>'+obj.len + "m"+'</td><td>'+obj.loop+'</td><tr>');
              i++;
                });

            console.log(data);

        },
        fail: function(error) {
            console.log(error);
        }
      })
    });

    $('#rename-form').submit(function(e){
      e.preventDefault();
      jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/rename',   //The server endpoint we are connecting to
        data: {
          filename: $('#panelformdiv').val(),
          newName: $('#rename-Box').val(),
          oldName: $('#oldname-Box').val()
        },
        success: function (data) {
            if (data.value == 1) {
              alert('file renamed successfully');
            } else {
              alert('file could not be renamed');
            }

        },
        fail: function(error) {
            console.log(error);
        }
      });
    });

    $('#show-data-form').submit(function(e){
      e.preventDefault();
      jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/showData',   //The server endpoint we are connecting to
        data: {
          filename: $('#panelformdiv').val(),
          name: $('#show-data-Box').val()
        },
        success: function (data) {
          let datastring = '';
            data.retData.forEach(function(obj){
              datastring = datastring + obj.Name + ' = ' + obj.Value + '\n';
            });
            alert(datastring);
            console.log(data.retData);
        },
        fail: function(error) {
            console.log(error);
        }
      });
    });

    // for readFile
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/filelog1',   //The server endpoint we are connecting to
        data: {
            // data1: "Value 1",
            // data2:1234.56
        },
      //  var list = new
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            let arr = data.return;
            arr.forEach(function(obj){
              $("#filelogpanel_tbody").append("<tr><td><a  href='"+obj.filename+
              "' download >"+obj.filename+'</a></td><td>'+obj.version+
              '</td><td>'+obj.creator+'</td><td>'+obj.numWaypoints+
              '</td><td>'+obj.numRoutes+'</td><td>'+obj.numTracks+'</td><tr>');
              $("#panelformdiv").append('<option value='+obj.filename+'>'+obj.filename+ '</option>');
              $("#panelformdiv2").append('<option value='+obj.filename+'>'+obj.filename+ '</option>');
            });
            // jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
             //We write the object to the console to show that the request was successful
            console.log(data);

        },
        fail: function(error) {
            console.log(error);
        }
    });


    $("#creategpx_form1").submit(function(e){
      e.preventDefault();
      let version = $('#entryBox3').val();
      if ($.isNumeric(version)==false) {
        alert('Version is not numeric');
      }else {
        jQuery.ajax({
          type: 'get',            //Request type
          dataType: 'json',       //Data type - we will use JSON for almost everything
          url: '/gpxcreater',   //The server endpoint we are connecting to
          data: {
              Filename : $('#entryBox1').val(),
               Creator : $('#entryBox2').val(),
              Version :$('#entryBox3').val()
          },
          success: function (data) {
              /*  Do something with returned object
                  Note that what we get is an object, not a string,
                  so we do not need to parse it on the server.
                  JavaScript really does handle JSONs seamlessly\
              */
              //
              // console.log("hello world");
              let condition  = data.return;
              // for i in ranges
              if (condition == 1) {
                alert('File created succesfuly !');
              }else {
                alert('File creation Failed !');
              }
                //$("#creategpx_form1").append('<option value='+obj.filename+'>'+obj.filename+ '</option>');
              // jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
               //We write the object to the console to show that the request was successful
              console.log(data);

          },
          fail: function(error) {
              // Non-200 return, do something with error
              $('#blah').html("On page load, received error from server");
              console.log(error);
          }
        });
      }

    });
    ///////////////////////////////////////////////////////////////////////////////////
    $("#latitudeForm").submit(function(e){
      e.preventDefault();
      let latitude = $('#latBox').val();
      let longitude = $('#lonBox').val();
      if ($.isNumeric(latitude)==false || $.isNumeric(longitude)==false) {
          alert('Latitude and Longitude can only be numeric');
      }else {
        // console.log( $('#panelformdiv2').val());
        //      console.log( $('#nameBox').val());
        //      console.log($('#latBox').val());
        //      console.log( $('#lonBox').val());
      //console.log( $('#panelformdiv').val());
      $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/addRoutes',   //The server endpoint we are connecting to
        data: {
            filename: $('#panelformdiv2').val(),
            name: $('#nameBox').val(),
            lat: $('#latBox').val(),
            lon: $('#lonBox').val()
        },
      //  var list = new
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            //  $("#panelformdiv2").append('<option value='+obj.filename+'>'+obj.filename+ '</option>');
              alert('File created succesfuly !');

            //console.log(data);

        },
        fail: function(error) {
            console.log(error);
        }
      })
    }
    });
    ////////////////////////////////////////////////////////////////////////////////////
    $("#pathbetweenForm").submit(function(e){
      e.preventDefault();
      Acc = $('#accBox').val();
      srcLa = $('#srclatBox').val();
      srcLo = $('#srclonBox').val();
      destla = $('#destlatBox').val();
      destlo =  $('#destlonBox').val();
      if ($.isNumeric(Acc)==false || $.isNumeric(srcLa)==false  || $.isNumeric(srcLo)==false
                                  || $.isNumeric(destla)==false || $.isNumeric(destlo)==false ) {
          alert('Accuracy,Latitude and Longitude values can only be numeric');
      }else{
      $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/pathBetween',   //The server endpoint we are connecting to
        data: {
            Accuracy: $('#accBox').val(),
            srcLat: $('#srclatBox').val(),
            srcLon: $('#srclonBox').val(),
            destlat: $('#destlatBox').val(),
            destlon: $('#destlonBox').val()
        },
      //  var list = new
        success: function (data) {

          let arr = data.routes;
          let i = 1;
          arr.forEach(function(obj){
            $("#findpath_tbody").append('<tr><td>'+"Route " +i+
            '</td><td>'+obj.name+'</td><td>'+obj.numPoints+
            '</td><td>'+obj.len + "m"+'</td><td>'+obj.loop+'</td></tr>');
            i++;
            //$("#panelformdiv").append('<option value='+obj.filename+'>'+obj.filename+ '</option>');
          });

          arr = data.tracks;
          i = 1;
          arr.forEach(function(obj){
            $("#findpath_tbody").append('<tr<td>'+"Track " +i+
            '</td><td>'+obj.name+'</td><td>'+obj.numPoints+
            '</td><td>'+obj.len + "m"+'</td><td>'+obj.loop+'</td><tr>');
            i++;
            //$("#panelformdiv").append('<option value='+obj.filename+'>'+obj.filename+ '</option>');
          });
          console.log(data);


        },
        fail: function(error) {
            console.log(error);
        }
      })
    }
    });
});
