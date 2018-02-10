(function(){
	$.fn.formPost = function(options)
    {
		var me = $(this);
        
		me.submit(function(event){
			event.preventDefault();
    		var data = {};
            
            me.find("input,select").each(function(index,input){
                var input = $(input);
                data[input.attr("name")] = input.val();
            });
            	
			$.post(options.action,JSON.stringify(data),function(data,status) {
				options.fn(data,status);
			});
		});
    };
})();
